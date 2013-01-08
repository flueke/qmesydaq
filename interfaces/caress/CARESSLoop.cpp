/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008,2011 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
 *   Copyright (C) 2009-2010 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/***************************************************************************
 * CARESS CORBA server for QMesyDAQ                                        *
 *                                                                         *
 * Note: You should have CARESS revision 1141 or later, ideal is revision  *
 *       1821 or later (load config before INIT with device kind 501)      *
 *       Earlier CARESS version have to use device kind 500 below          *
 *                                                                         *
 * config line in file "hardware_modules_*.dat":                           *
 *                                                                         *
 * ; simple counters                                                       *
 * ;name kind CORBA-ref               type                                 *
 * TIM1  501  qmesydaq.caress_object  timer          <optional scale>      *
 * MON1  501  qmesydaq.caress_object  monitor1                             *
 * MON2  501  qmesydaq.caress_object  monitor2                             *
 * MON3  501  qmesydaq.caress_object  monitor3                             *
 * MON4  501  qmesydaq.caress_object  monitor4                             *
 * EVENT 501  qmesydaq.caress_object  event                                *
 *                                                                         *
 * ; special counters / detectors                                          *
 * ;name kind CORBA-ref               type           ignore  width  height *
 * ADET  501  qmesydaq.caress_object  histogram      0       112    112    *
 * ;name kind CORBA-ref               type           ignore  width         *
 * LDET  501  qmesydaq.caress_object  diffractogram  0       112           *
 * ;name kind CORBA-ref               type           channel width         *
 * LDET  501  qmesydaq.caress_object  spectrogram    0       112           *
 *                                                                         *
 * description of special counter types:                                   *
 * - histogram:     a 2D histogram over the complete detector scaled to    *
 *                  selected width and height                              *
 * - diffractogram: counts of each channel as a linear detector            *
 * - spectrogram:   one selected channel, the bins as linear detector      *
 *                  use "all" or -1 to select the sum of all channels      *
 ***************************************************************************/

#include "CARESSLoop.h"
#include "mapcorrect.h"
#include "mapcorrectparser.h"
#include "logging.h"

#include <QApplication>
#include <QDebug>
#include <QStringList>
#include <QMutexLocker>
#include <QTime>
#if defined(_MSC_VER)
#include "stdafx.h"
#else
#include <unistd.h>
#endif

// omniORB needs to know the platform as define __??__
#if defined(__amd64) || defined(__amd64__) || defined(amd64) || defined(__x86_64)
#ifndef __x86_64__
#define __x86_64__
#endif
#elif defined(__i386) || defined(__i386__) || defined(i386)
#ifndef __x86__
#define __x86__
#endif
#endif

#include <omniORB4/CORBA.h>
#include "corbadevice.h"

#include "CommandEvent.h"
#include "measurement.h"
#include "MultipleLoopApplication.h"
#include "QMesydaqDetectorInterface.h"

/***************************************************************************
 * defines and constants
 ***************************************************************************/

#define ARRAY_SIZE(x) ((int)(sizeof(x)/sizeof((x)[0])))

//! \brief default time factor (divider) for timer device (CARESS uses an integer data type for this)
const double DEFAULTTIMEFACTOR = 100.0; // default CARESS factor for timer (Hertz)

//! \brief CARESS status values
enum {
  // status after init
  OFF_LINE      = 0, /*!< device is offline */
  ON_LINE       = 1, /*!< device is online */
  MANUAL_MODE   = 2, /*!< motor is in manual module */
  NOT_DEFINED   = 3, /*!< module was not defined */
  LIMIT_CONTACT = 4, /*!< motor is at a limit contact */

  // normal status for other functions
  NOT_ACTIVE    = 1, /*!< device is unused */
  ACTIVE        = 2, /*!< module is moving */
  DONE          = 3, /*!< module reached target or aborted move and is now idle */
  LOADED        = 4, /*!< counter was loaded with new data */
  ACTIVE1       = 5, /*!< like ACTIVE - but be used as special status */
  COMBO_ACTIVE  = 6, /*!< multiplexer is active */
  MODULE_ERROR  = 7, /*!< module error text available */
  MODULE_RESET  = 8  /*!< fatal error, module has to be reset */
};

//! \brief mapping of QMesyDAQ devices into CARESS CORBA device arrays
enum {
  QMESYDAQ_MON1 = 0,      //!< monitor counter 1
  QMESYDAQ_MON2,          //!< monitor counter 2
  QMESYDAQ_MON3,          //!< monitor counter 3
  QMESYDAQ_MON4,          //!< monitor counter 4
  QMESYDAQ_EVENT,         //!< event counter
  QMESYDAQ_TIMER,         //!< timer
  QMESYDAQ_HISTOGRAM,     //!< mapped histogram
  QMESYDAQ_DIFFRACTOGRAM, //!< diffractogram
  QMESYDAQ_SPECTROGRAM,   //!< spectogram of one detector channel
  //QMESYDAQ_ADC1,          //!< analog input 1
  //QMESYDAQ_ADC2,          //!< analog input 2
  //QMESYDAQ_TTL1,          //!< digital input 1
  //QMESYDAQ_TTL2,          //!< digital input 2
  QMESYDAQ_MAXDEVICES
};
static const char* g_asDevices[]={"monitor_1","monitor_2","monitor_3","monitor_4","event_counter","timer","histogram","diffractogram"};

/*!
  \brief CORBA server implementing the "CARESS CORBA device"
     (IDL interface CARESS::CORBADevice)
  \class CORBADevice_i
 */
class CORBADevice_i: public POA_CARESS::CORBADevice, public PortableServer::RefCountServantBase
{
public:
  // standard constructor
  CORBADevice_i(MultipleLoopApplication* pApp);
  virtual ~CORBADevice_i();

  // methods corresponding to defined IDL attributes and operations
  CARESS::ReturnType init_module(CORBA::Long kind, CORBA::Long id, const char* config_line, CORBA::Long& module_status);
  CARESS::ReturnType init_module_ex(CORBA::Long kind, CORBA::Long id, const char* name, const char* config_line, CORBA::Long& module_status, CORBA::String_out description);
  CARESS::ReturnType release_module(CORBA::Long kind, CORBA::Long id);
  CARESS::ReturnType start_module(CORBA::Long kind, CORBA::Long id, CORBA::Long run_no, CORBA::Long mesr_count, CORBA::Long& module_status);
  CARESS::ReturnType stop_module(CORBA::Long kind, CORBA::Long id, CORBA::Long& module_status);
  CARESS::ReturnType drive_module(CORBA::Long kind, CORBA::Long id, const CARESS::Value& data, CORBA::Long& calculated_timeout, CORBA::Boolean& delay, CORBA::Long& module_status);
  CARESS::ReturnType load_module(CORBA::Long kind, CORBA::Long id, const CARESS::Value& data, CORBA::Long& module_status);
  CARESS::ReturnType loadblock_module(CORBA::Long kind, CORBA::Long id, CORBA::Long start_channel, CORBA::Long end_channel, CORBA::Long& module_status, const CARESS::Value& data);
  CARESS::ReturnType read_module(CORBA::Long kind, CORBA::Long id, CORBA::Long& module_status, CARESS::Value_out data);
  CARESS::ReturnType readblock_params(CORBA::Long kind, CORBA::Long id, CORBA::Long& start_channel, CORBA::Long& end_channel, CARESS::DataType& type);
  CARESS::ReturnType readblock_module(CORBA::Long kind, CORBA::Long id, CORBA::Long start_channel, CORBA::Long end_channel, CORBA::Long& module_status, CARESS::Value_out data);

  CORBA::Boolean is_readable_module(CORBA::Long id);
  CORBA::Boolean is_drivable_module(CORBA::Long id);
  CORBA::Boolean is_counting_module(CORBA::Long id);
  CORBA::Boolean is_status_module(CORBA::Long id);
  CORBA::Boolean needs_reference_module(CORBA::Long id);
  CARESS::Value* get_attribute(CORBA::Long id, const char* name);
  void           set_attribute(CORBA::Long id, const char* name, const CARESS::Value& data);

private:
  MultipleLoopApplication* m_theApp;
  QMutex  m_mutex;

protected:
  long    m_lHistogramX;         //!< width of histogram
  long    m_lHistogramY;         //!< height of histogram
  long    m_lDiffractogramWidth; //!< width of diffractogram
  long    m_lSpectrogramChannel; //!< selected spectrogram channel
  long    m_lSpectrogramWidth;   //!< width of spectrogram
  long    m_lRunNo;              //!< current/last CARESS run number
  long    m_lStepNo;             //!< current/last CARESS measurment step
  long    m_lMesrCount;          //!< current/last CARESS resolution step (not used)
  bool    m_bListmode;           //!< true, if QMesyDAQ should acquire a list file
  QString m_sListfile;           //!< list file name
  double  m_dblTimerScale;       //!< override for DEFAULTTIMEFACTOR
  long    m_lSourceChannels;     //!< used for mapping: number of MPSD-or-something channels

  long    m_lId[QMESYDAQ_MAXDEVICES];    //!< CARESS ids of internal devices
  bool    m_b64Bit[QMESYDAQ_MAXDEVICES]; //!< 64-bit mode for internal devices
  int     m_iMaster;                     //!< which internal device is the master counter

  char    m_szErrorMessage[64];          //!< last error message text
  QList<quint64> m_aullDetectorData;     //!< last histogram/diffractogram/spectrogram
  int            m_iDetectorWidth;       //!< last width of histogram/diffractogram/spectrogram
};

/***************************************************************************
 * CARESSLoop is the connection between QMesyDAQ and CORBA server
 ***************************************************************************/
static CORBA::Boolean bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_ptr objref, const char* szName);

/*!
  \brief constructor
  \param[in] argList   command line arguments of QMesyDAQ
  \param[in] interface QtInterface for data exchange between QMesyDAQ and CARESS interface
 */
CARESSLoop::CARESSLoop(QStringList argList, QtInterface *interface)
  : m_bDoLoop(true), m_asArguments(argList)
{
  (void)interface;

  setObjectName("CARESSLoop");
  // testing
}

//! \brief CARESSLoop is derived from QThread and if this function returns, the thread exits
void CARESSLoop::runLoop()
{
  MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
  if (!app)
  {
    MSG_ERROR << "not a MultipleLoopApplication";
    return;
  }

  QList<char*> args1;
  char** args2=NULL;
  int i;
  const char* szName=NULL;

  args2=new char*[m_asArguments.count()+1];
  for (i=0; i<m_asArguments.count(); ++i)
  {
    QByteArray arg=m_asArguments[i].toLatin1();
    const char* sz1=arg.constBegin();
    int iLen=arg.size();
    char* sz2=new char[iLen+1];
    if (!sz2) break;
    memmove(sz2,sz1,iLen);
    sz2[iLen]='\0';
    args1.push_back(sz2);
    args2[i]=sz2;
  }
  if (i<m_asArguments.count() || m_asArguments.count()!=args1.count())
  {
    delete[] args2;
    for (i=0; i<args1.count(); ++i)
      delete[] args1[i];
    MSG_ERROR << "cannot copy argument list";
    return;
  }

  try
  {
    // initialize CORBA ORB
    int iDummyArgc=m_asArguments.count();
    CORBA::ORB_var orb = CORBA::ORB_init(iDummyArgc,args2);

    // Obtain a reference to the root POA.
    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);

    // We allocate the objects on the heap.  Since these are reference
    // counted objects, they will be deleted by the POA when they are no
    // longer needed.
    CORBADevice_i* myCORBADevice_i = new CORBADevice_i(app);

    // Activate the objects.  This tells the POA that the objects are
    // ready to accept requests.
    PortableServer::ObjectId_var myCORBADevice_iid = poa->activate_object(myCORBADevice_i);

    // Obtain a reference to the object, and register it in
    // the naming service.
    obj = myCORBADevice_i->_this();

    // Obtain a reference to each object and output the stringified
    // IOR to stdout
    CORBA::Object_var ref = myCORBADevice_i->_this();
    CORBA::String_var ior = orb->object_to_string(ref);
    MSG_DEBUG << "IDL object CORBADevice (version " << (const char*)(VERSION) << " ) IOR = " << ior.in();

    for (i=1; i<iDummyArgc; ++i)
    {
      char* szArg=args2[i];
      char* szOpt=(char*)strchr(szArg,'=');
      if (szOpt==NULL)
      {
        if (szArg[0]=='-' && (i+1)<iDummyArgc)
          szOpt=args2[++i];
      }
      else
        *szOpt++='\0';
      if (strcmp(szArg,"-n")==0 && szOpt!=NULL && *szOpt!='\0')
        szName=szOpt;
    }
    if (!bindObjectToName(orb,obj,szName))
      MSG_WARNING << "warning: cannot bind to naming service";

    delete[] args2;
    for (i=0; i<args1.count(); ++i)
      delete[] args1[i];

    myCORBADevice_i->_remove_ref();

    // Obtain a POAManager, and tell the POA to start accepting
    // requests on its objects.
    PortableServer::POAManager_var pman = poa->the_POAManager();
    pman->activate();

    // allow a clean CORBA shutdown instead of simply call "orb->run();"
    connect(app,SIGNAL(aboutToQuit()),this,SLOT(shutdownLoop()),Qt::QueuedConnection);
    while (m_bDoLoop)
    {
      if (orb->work_pending())
        orb->perform_work();
      usleep(1000);
    }
    orb->destroy();
  }
  catch (CORBA::SystemException&)
  {
    MSG_ERROR << "Caught CORBA::SystemException.";
  }
  catch (CORBA::Exception&)
  {
    MSG_ERROR << "Caught CORBA::Exception.";
  }
  catch (omniORB::fatalException& fe)
  {
    MSG_ERROR << "Caught omniORB::fatalException.";
  }
  catch (...)
  {
    MSG_ERROR << "Caught unknown exception.";
  }
}

/*!
  \brief store object reference with a name into CORBA name service
  \param[in] orb     use this CORBA ORB
  \param[in] objref  CORBA object reference
  \param[in] szName  name for name service
  \return 1 = successful, 0 = error
 */
static CORBA::Boolean bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_ptr objref, const char* szName)
{
  CosNaming::NamingContext_var rootContext;

  try
  {
    // Obtain a reference to the root context of the Name service:
    CORBA::Object_var obj;
    obj = orb->resolve_initial_references("NameService");

    // Narrow the reference returned.
    rootContext = CosNaming::NamingContext::_narrow(obj);
    if (CORBA::is_nil(rootContext))
    {
      MSG_ERROR << "Failed to narrow the root naming context.";
      return 0;
    }
  }
  catch (CORBA::ORB::InvalidName&)
  {
    // This should not happen!
    MSG_ERROR << "Service required is invalid [does not exist].";
    return 0;
  }
  catch (...)
  {
    // This should not happen!
    MSG_ERROR << "Caught unknown exception.";
    return 0;
  }

  try
  {
    // Bind objref with name Echo to the testContext:
    CosNaming::Name objectName;
    objectName.length(1);
    objectName[0].id = szName ? szName : "qmesydaq";   // string copied
    objectName[0].kind = (const char*)"caress_object"; // string copied

    try
    {
      rootContext->bind(objectName, objref);
    }
    catch (CosNaming::NamingContext::AlreadyBound&)
    {
      rootContext->rebind(objectName, objref);
    }
    // Note: Using rebind() will overwrite any Object previously bound
    //       to /test/Echo with obj.
    //       Alternatively, bind() can be used, which will raise a
    //       CosNaming::NamingContext::AlreadyBound exception if the name
    //       supplied is already bound to an object.
  }
  catch (CORBA::COMM_FAILURE&)
  {
    MSG_ERROR << "Caught system exception COMM_FAILURE -- unable to contact the naming service.";
    return 0;
  }
  catch (CORBA::SystemException&)
  {
    MSG_ERROR << "Caught a CORBA::SystemException while using the naming service.";
    return 0;
  }
  catch (...)
  {
    // This should not happen!
    MSG_ERROR << "Caught unknown exception.";
    return 0;
  }
  return 1;
}

/***************************************************************************
 * implementation of the CARESS CORBA server
 ***************************************************************************/

/*!
  \brief constructor
  \param[in] pApp reference to QMesyDAQ application
 */
CORBADevice_i::CORBADevice_i(MultipleLoopApplication *pApp) :
  m_theApp(pApp), m_lHistogramX(0), m_lHistogramY(0), m_lDiffractogramWidth(0),
  m_lSpectrogramChannel(-1), m_lSpectrogramWidth(0),
  m_lRunNo(0), m_lStepNo(0), m_lMesrCount(-1), m_bListmode(false),
  m_dblTimerScale(DEFAULTTIMEFACTOR), m_lSourceChannels(-1), m_iMaster(-1), m_iDetectorWidth(0)
{
  memset(&m_lId[0],0,sizeof(m_lId));
  memset(&m_b64Bit[0],0,sizeof(m_b64Bit));
  memset(&m_szErrorMessage[0],0,sizeof(m_szErrorMessage));
}

//! \brief destructor
CORBADevice_i::~CORBADevice_i()
{
}

/*!
  \brief initialisation of device (old interface, see also "init_module_ex")
  \param[in]  kind           kind of initialisation
  \param[in]  id             CARESS id
  \param[in]  config_line    text line from "hardware_modules.dat" file
                 see head of source file about content of this line
  \param[out] module_status  device status
  \note this function calls \c init_module_ex only. Although this may enable
    exceptions, this implementation uses exceptions for \c get_attribute and
    \c set_attribute only.
 */
CARESS::ReturnType CORBADevice_i::init_module(CORBA::Long kind,
                                              CORBA::Long id,
                                              const char* config_line,
                                              CORBA::Long& module_status)
{
  CORBA::String_var desc;
  return init_module_ex(kind,id,NULL,config_line,module_status,desc.out());
}

/*!
  \brief initialisation of device (old interface, see also "init_module_ex")
  \param[in]  kind           kind of initialisation
  \param[in]  id             CARESS id
  \param[in]  name           CARESS name of this device
  \param[in]  config_line    text line from "hardware_modules.dat" file
                 see head of source file about content of this line
  \param[out] module_status  device status
  \param[out] description    error description or empty string
  \return CARESS::OK = successful \n CARESS::NOT_OK = error
  \note Although this function may enable exceptions, this implementation
    uses exceptions for \c get_attribute and \c set_attribute only.
 */
CARESS::ReturnType CORBADevice_i::init_module_ex(CORBA::Long kind,
                                                 CORBA::Long id,
                                                 const char* name,
                                                 const char* config_line,
                                                 CORBA::Long& module_status,
                                                 CORBA::String_out description)
{
  QMutexLocker lock(&m_mutex);
  try
  {
    (void)name;
    MSG_DEBUG << "init(kind=" << kind << ", id=" << id << ", name=" << name << ", config_line=" << config_line << ')';
    if ((kind!=0/*INIT*/ && kind!=8/*RESET*/) || id<1)
      throw ((const char*)"invalid init kind or id");

    const char* ptr1=config_line;
    int i,iDevice=-1;
    QMesyDAQDetectorInterface* pInterface=NULL;
    if (m_theApp!=NULL)
      pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());

    while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
    i=ptr1-config_line;
    while (ptr1[0]==' ' || ptr1[0]=='\t') ++ptr1;

    if      ((i== 7 && strncasecmp(config_line,"monitor",i)==0)       || (i==3 && strncasecmp(config_line,"mon",i)==0) ||
             (i== 8 && strncasecmp(config_line,"monitor1",i)==0)      || (i==4 && strncasecmp(config_line,"mon1",i)==0))  iDevice=QMESYDAQ_MON1;
    else if ((i== 8 && strncasecmp(config_line,"monitor2",i)==0)      || (i==4 && strncasecmp(config_line,"mon2",i)==0))  iDevice=QMESYDAQ_MON2;
    else if ((i== 8 && strncasecmp(config_line,"monitor3",i)==0)      || (i==4 && strncasecmp(config_line,"mon3",i)==0))  iDevice=QMESYDAQ_MON3;
    else if ((i== 8 && strncasecmp(config_line,"monitor4",i)==0)      || (i==4 && strncasecmp(config_line,"mon4",i)==0))  iDevice=QMESYDAQ_MON4;
    else if ((i== 5 && strncasecmp(config_line,"event",i)==0)         || (i==3 && strncasecmp(config_line,"evt",i)==0))   iDevice=QMESYDAQ_EVENT;
    else if ((i== 5 && strncasecmp(config_line,"timer",i)==0)         || (i==3 && strncasecmp(config_line,"tim",i)==0))   iDevice=QMESYDAQ_TIMER;
    else if ((i== 9 && strncasecmp(config_line,"histogram",i)==0)     || (i==5 && strncasecmp(config_line,"histo",i)==0)) iDevice=QMESYDAQ_HISTOGRAM;
    else if ((i==13 && strncasecmp(config_line,"diffractogram",i)==0) || (i==4 && strncasecmp(config_line,"diff",i)==0))  iDevice=QMESYDAQ_DIFFRACTOGRAM;
    else if ((i==11 && strncasecmp(config_line,"spectrogram",i)==0)   || (i==4 && strncasecmp(config_line,"spec",i)==0))  iDevice=QMESYDAQ_SPECTROGRAM;
    else throw ((const char*)"invalid counter type");

    if (iDevice!=QMESYDAQ_HISTOGRAM     && m_lId[QMESYDAQ_HISTOGRAM]    ==id) m_lId[QMESYDAQ_HISTOGRAM]    =0;
    if (iDevice!=QMESYDAQ_DIFFRACTOGRAM && m_lId[QMESYDAQ_DIFFRACTOGRAM]==id) m_lId[QMESYDAQ_DIFFRACTOGRAM]=0;
    if (iDevice!=QMESYDAQ_SPECTROGRAM   && m_lId[QMESYDAQ_SPECTROGRAM]  ==id) m_lId[QMESYDAQ_SPECTROGRAM]  =0;
    m_lId[iDevice]=0;
    m_b64Bit[iDevice]=false;
    if (m_iMaster==iDevice) m_iMaster=-1;

    switch (iDevice)
    {
      case QMESYDAQ_TIMER:
      {
        const char* ptr2;
        char* ptr3;

        // timer scaler/factor
        ptr2=ptr1;
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        ptr3=(char*)ptr2;
        m_dblTimerScale=strtod(ptr2,&ptr3);
        if (ptr3==NULL || ptr2>=ptr3 || (ptr3[0]!=' ' && ptr3[0]!='\t' && ptr3[0]!='\0')) m_dblTimerScale=DEFAULTTIMEFACTOR;
        else if (m_dblTimerScale<=0.0) m_dblTimerScale=DEFAULTTIMEFACTOR;
        MSG_DEBUG << "init(timer factor " << m_dblTimerScale << ')';
        break;
      }
      case QMESYDAQ_HISTOGRAM:
      {
        const char* ptr2;
        char* ptr3;
        m_lHistogramX=m_lHistogramY=0;
        // skip next value
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        while (ptr1[0]==' ' || ptr1[0]=='\t') ++ptr1;

        // width of scaled histogram
        while (ptr1[0]=='0' && ptr1[0]>='0' && ptr1[0]<='9') ++ptr1;
        ptr2=ptr1;
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        ptr3=(char*)ptr2;
        m_lHistogramX=strtol(ptr2,&ptr3,0);
        if (ptr3==NULL || ptr2>=ptr3 || (ptr3[0]!=' ' && ptr3[0]!='\t' && ptr3[0]!='\0')) m_lHistogramX=0;
        else if (m_lHistogramX==0 && pInterface!=NULL)
        {
          quint16 w=0,h=0;
          pInterface->readHistogramSize(w,h);
          m_lHistogramX=w;
        }
        if (m_lHistogramX<1)
        {
          m_lId[QMESYDAQ_HISTOGRAM]=0;
          throw ((const char*)"invalid histogram width");
        }
        ptr1=ptr3;
        while (ptr1[0]==' ' || ptr1[0]=='\t') ++ptr1;

        // height of scaled histogram
        while (ptr1[0]=='0' && ptr1[0]>='0' && ptr1[0]<='9') ++ptr1;
        ptr2=ptr1;
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        ptr3=(char*)ptr2;
        m_lHistogramY=strtol(ptr2,&ptr3,0);
        if (ptr3==NULL || ptr2>=ptr3 || (ptr3[0]!=' ' && ptr3[0]!='\t' && ptr3[0]!='\0')) m_lHistogramY=0;
        else if (m_lHistogramY==0 && pInterface!=NULL)
        {
          quint16 w=0,h=0;
          pInterface->readHistogramSize(w,h);
          m_lHistogramY=h;
        }
        if (m_lHistogramY<1)
        {
          m_lId[QMESYDAQ_HISTOGRAM]=0;
          throw ((const char*)"invalid histogram height");
        }

        // number of source channels for mapping of scaled histogram
        ptr1=ptr3;
        while (ptr1[0]==' ' || ptr1[0]=='\t') ++ptr1;
        while (ptr1[0]=='0' && ptr1[0]>='0' && ptr1[0]<='9') ++ptr1;
        ptr2=ptr1;
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        ptr3=(char*)ptr2;
        m_lSourceChannels=strtol(ptr2,&ptr3,0);
        if (ptr3==NULL || ptr2>=ptr3 || (ptr3[0]!=' ' && ptr3[0]!='\t' && ptr3[0]!='\0')) m_lSourceChannels=-1;

        if (pInterface!=NULL)
          pInterface->updateMainWidget(m_lHistogramX,m_lHistogramY,-1);
        MSG_DEBUG << "init(histogram)";
        break;
      }
      case QMESYDAQ_DIFFRACTOGRAM:
      {
        const char* ptr2;
        char* ptr3;
        m_lDiffractogramWidth=0;
        // skip next value
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        while (ptr1[0]==' ' || ptr1[0]=='\t') ++ptr1;

        // width of scaled histogram
        while (ptr1[0]=='0' && ptr1[0]>='0' && ptr1[0]<='9') ++ptr1;
        ptr2=ptr1;
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        ptr3=(char*)ptr2;
        m_lDiffractogramWidth=strtol(ptr2,&ptr3,0);
        if (ptr3==NULL || ptr2>=ptr3 || (ptr3[0]!=' ' && ptr3[0]!='\t')) m_lDiffractogramWidth=0;
        else if (m_lDiffractogramWidth==0 && pInterface!=NULL)
        {
          QList<quint64> tmp=pInterface->readDiffractogram();
          m_lDiffractogramWidth=tmp.count();
        }
        if (m_lDiffractogramWidth<1)
        {
          m_lId[QMESYDAQ_DIFFRACTOGRAM]=0;
          throw ((const char*)"invalid diffractogram width");
        }

        // number of source channels for mapping of scaled histogram
        ptr1=ptr3;
        while (ptr1[0]==' ' || ptr1[0]=='\t') ++ptr1;
        while (ptr1[0]=='0' && ptr1[0]>='0' && ptr1[0]<='9') ++ptr1;
        ptr2=ptr1;
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        ptr3=(char*)ptr2;
        m_lSourceChannels=strtol(ptr2,&ptr3,0);
        if (ptr3==NULL || ptr2>=ptr3 || (ptr3[0]!=' ' && ptr3[0]!='\t' && ptr3[0]!='\0')) m_lSourceChannels=-1;

        if (pInterface!=NULL && m_lId[QMESYDAQ_HISTOGRAM]<1 && m_lId[QMESYDAQ_SPECTROGRAM]<1)
          pInterface->updateMainWidget(m_lDiffractogramWidth,0,-1);
        MSG_DEBUG << "init(diffractogram)";
        break;
      }
      case QMESYDAQ_SPECTROGRAM:
      {
        const char* ptr2;
        char* ptr3;
        m_lSpectrogramChannel=-2;
        m_lSpectrogramWidth=0;
        // select spectrogram channel
        while (ptr1[0]=='0' && ptr1[0]>='0' && ptr1[0]<='9') ++ptr1;
        ptr2=ptr1;
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        i=ptr1-ptr2;
        if (i>=3 && (strncasecmp(ptr2,"all",3)==0 || strncasecmp(ptr2,"sum",3)==0))
          m_lSpectrogramChannel=-1;
        else
        {
          m_lSpectrogramChannel=strtol(ptr2,&ptr3,0);
          if (ptr3==NULL || ptr2>=ptr3 || (ptr3[0]!=' ' && ptr3[0]!='\t')) m_lSpectrogramChannel=-2;
        }
        while (ptr1[0]==' ' || ptr1[0]=='\t') ++ptr1;

        // width of scaled spectrogram
        while (ptr1[0]=='0' && ptr1[0]>='0' && ptr1[0]<='9') ++ptr1;
        ptr2=ptr1;
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        ptr3=(char*)ptr2;
        m_lSpectrogramWidth=strtol(ptr2,&ptr3,0);
        if (ptr3==NULL || ptr2>=ptr3 || (ptr3[0]!=' ' && ptr3[0]!='\t')) m_lSpectrogramWidth=0;
        else if (m_lSpectrogramWidth==0 && pInterface!=NULL)
        {
          QList<quint64> tmp=pInterface->readSpectrogram(m_lSpectrogramChannel);
          m_lSpectrogramWidth=tmp.size();
        }
        if (m_lSpectrogramChannel<-1 || m_lSpectrogramWidth<1)
        {
          m_lId[QMESYDAQ_SPECTROGRAM]=0;
          throw ((const char*)"invalid spectrogram width");
        }

        // number of source channels for mapping of scaled histogram
        ptr1=ptr3;
        while (ptr1[0]==' ' || ptr1[0]=='\t') ++ptr1;
        while (ptr1[0]=='0' && ptr1[0]>='0' && ptr1[0]<='9') ++ptr1;
        ptr2=ptr1;
        while (ptr1[0]!=' ' && ptr1[0]!='\t' && ptr1[0]!='\0') ++ptr1;
        ptr3=(char*)ptr2;
        m_lSourceChannels=strtol(ptr2,&ptr3,0);
        if (ptr3==NULL || ptr2>=ptr3 || (ptr3[0]!=' ' && ptr3[0]!='\t' && ptr3[0]!='\0')) m_lSourceChannels=-1;

        if (pInterface!=NULL && m_lId[QMESYDAQ_HISTOGRAM]<1)
          pInterface->updateMainWidget(m_lSpectrogramWidth,m_lSpectrogramChannel,-1);
        MSG_DEBUG << "init(spectrogram)";
        break;
      }
      default:
        if (iDevice<ARRAY_SIZE(g_asDevices))
          MSG_DEBUG << "init(counter " << g_asDevices[iDevice] << ')';
        else
          MSG_DEBUG << "init(counter " << id << ')';
        if (pInterface!=NULL && m_lId[QMESYDAQ_HISTOGRAM]<1 && m_lId[QMESYDAQ_SPECTROGRAM]<1 && m_lId[QMESYDAQ_DIFFRACTOGRAM]<1)
          pInterface->updateMainWidget(0,0,-1);
        break;
    }
    m_lId[iDevice]=id;
    m_lStepNo=0;

    module_status=ON_LINE;
    description=CORBA::string_dup("");
    m_szErrorMessage[0]='\0';
    return CARESS::OK;
  }
  catch (const char* msg)
  {
    MSG_ERROR << "init - " << msg;
    description=CORBA::string_dup(msg);
    strncpy(m_szErrorMessage,description,sizeof(m_szErrorMessage));
    m_szErrorMessage[sizeof(m_szErrorMessage)-1]='\0';
    return CARESS::NOT_OK;
  }
  catch (...)
  {
    MSG_ERROR << "init - Caught unknown exception.";
    description=CORBA::string_dup("unknown exception");
    strncpy(m_szErrorMessage,description,sizeof(m_szErrorMessage));
    m_szErrorMessage[sizeof(m_szErrorMessage)-1]='\0';
    return CARESS::NOT_OK;
  }
}

/*!
  \brief cleanup device
  \param[in]  kind  ignore this parameter
  \param[in]  id    CARESS id
 */
CARESS::ReturnType CORBADevice_i::release_module(CORBA::Long kind,
                                                 CORBA::Long id)
{
  QMutexLocker lock(&m_mutex);
  QMesyDAQDetectorInterface* pInterface=NULL;
  int iDevice;

  (void)kind;
  if (m_theApp!=NULL)
    pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());

  for (iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
    if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
      break;
  m_lId[iDevice]=0;
  m_b64Bit[iDevice]=false;
  MSG_DEBUG << "release(kind=" << kind << ", id=" << id << ')';
  m_szErrorMessage[0]='\0';
  if (pInterface!=NULL)
  {
    switch (iDevice)
    {
      case QMESYDAQ_HISTOGRAM:
        if (m_lId[QMESYDAQ_SPECTROGRAM]>0) pInterface->updateMainWidget(m_lSpectrogramWidth,m_lSpectrogramChannel,-1);
        else if (m_lId[QMESYDAQ_DIFFRACTOGRAM]>0) pInterface->updateMainWidget(m_lSpectrogramWidth,m_lSpectrogramChannel,-1);
        else pInterface->updateMainWidget(0,0,-1);
        break;
      case QMESYDAQ_DIFFRACTOGRAM:
        if (m_lId[QMESYDAQ_HISTOGRAM]>0) pInterface->updateMainWidget(m_lHistogramX,m_lHistogramY,-1);
        else if (m_lId[QMESYDAQ_SPECTROGRAM]>0) pInterface->updateMainWidget(m_lSpectrogramWidth,m_lSpectrogramChannel,-1);
        else pInterface->updateMainWidget(0,0,-1);
        break;
      case QMESYDAQ_SPECTROGRAM:
        if (m_lId[QMESYDAQ_HISTOGRAM]>0) pInterface->updateMainWidget(m_lHistogramX,m_lHistogramY,-1);
        else if (m_lId[QMESYDAQ_DIFFRACTOGRAM]>0) pInterface->updateMainWidget(m_lSpectrogramWidth,m_lSpectrogramChannel,-1);
        else pInterface->updateMainWidget(0,0,-1);
        break;
    }
  }
  return CARESS::OK;
}

/*!
  \brief start new measurment (-step)
  \param[in]  kind           kind of start to distinguish between new step or measurment
  \param[in]  id             CARESS id
  \param[in]  run_no         CARESS run number
  \param[in]  mesr_count     CARESS resolution step (not used)
  \param[out] module_status  current device status
 */
CARESS::ReturnType CORBADevice_i::start_module(CORBA::Long kind,
                                               CORBA::Long id,
                                               CORBA::Long run_no,
                                               CORBA::Long mesr_count,
                                               CORBA::Long& module_status)
{
  QMutexLocker lock(&m_mutex);
  MSG_DEBUG << "start(kind=" << kind << ", id=" << id << ", run_no=" << run_no << ", mesr_count=" << mesr_count << ", module_status=ACTIVE)";
  m_szErrorMessage[0]='\0';
  try
  {
    QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
    int iDevice;

    if (!pInterface)
    {
      strcpy(m_szErrorMessage,"control interface not initialized");
      MSG_DEBUG << "start - " << (const char*)m_szErrorMessage;
      module_status=OFF_LINE;
      return CARESS::NOT_OK;
    }

    for (iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
      if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
        break;

    switch (iDevice)
    {
      case QMESYDAQ_HISTOGRAM:
        pInterface->updateMainWidget(m_lHistogramX,m_lHistogramY,run_no);
        break;
      case QMESYDAQ_SPECTROGRAM:
        if (m_lId[QMESYDAQ_HISTOGRAM]<1)
          pInterface->updateMainWidget(m_lSpectrogramWidth,m_lSpectrogramChannel,run_no);
        break;
      case QMESYDAQ_DIFFRACTOGRAM:
        if (m_lId[QMESYDAQ_HISTOGRAM]<1 && m_lId[QMESYDAQ_SPECTROGRAM]<1)
          pInterface->updateMainWidget(m_lDiffractogramWidth,0,run_no);
        break;
    }

    if (m_iMaster<0 || iDevice==m_iMaster) // no known master or this device is the master
    {
      m_lRunNo=run_no;
      m_lMesrCount=mesr_count;
      pInterface->setRunID(m_lRunNo,false);
      if (kind==0)
      {
        if (mesr_count==0) ++m_lStepNo;
        if (m_bListmode && m_sListfile.isEmpty())
        {
          QString sName;
          sName.sprintf("car_listmode_r%05ld_s%03ld.mdat",m_lRunNo,m_lMesrCount);
          pInterface->setListFileName(sName);
        }
      }
      if (pInterface->status()==0)
      {
        QTime t1;
        if (kind==1)
          pInterface->resume();
        else
          pInterface->start();
        t1=QTime::currentTime();
        for (;;)
        {
          int tDiff;
          usleep(1000);
          if (pInterface->status()!=0) break;
          tDiff=t1.msecsTo(QTime::currentTime());
          if (tDiff<0) tDiff+=86400000;
          if (tDiff>1000) break;
        }
      }
      if (iDevice<ARRAY_SIZE(g_asDevices))
        MSG_DEBUG << "start device " << g_asDevices[iDevice];
      else
        MSG_DEBUG << "start device " << iDevice;
    }
    module_status=ACTIVE;
    return CARESS::OK;
  }
  catch (...)
  {
    MSG_ERROR << "start - Caught unknown exception.";
    return CARESS::NOT_OK;
  }
}

/*!
  \brief stop measurment
  \param[in]  kind           kind of stop to distinguish between pause or measurment end
  \param[in]  id             CARESS id
  \param[out] module_status  current device status
 */
/***************************************************************************
 * stop measurment
 ***************************************************************************/
CARESS::ReturnType CORBADevice_i::stop_module(CORBA::Long kind,
                                              CORBA::Long id,
                                              CORBA::Long& module_status)
{
  QMutexLocker lock(&m_mutex);
  MSG_DEBUG << "stop(all=" << (const char*)(((kind>>31)&1)?"yes":"no") << ", kind=" << (kind&0x7FFFFFFF) << ", id=" << id << ')';
  m_szErrorMessage[0]='\0';
  try
  {
    QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
    int iDevice;

    if (!pInterface)
    {
      strcpy(m_szErrorMessage,"control interface not initialized");
      MSG_DEBUG << "stop - " << (const char*)m_szErrorMessage;
      module_status=OFF_LINE;
      return CARESS::NOT_OK;
    }

    if (kind==0/*PAUSE*/ || kind==1/*END-OF-MEASURMENT*/)
    {
      if (kind==1) m_lStepNo=0;

      for (iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
        if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
          break;

      if (m_iMaster<0 || iDevice>=0) // no master or this device is the master
      {
        if (pInterface->status()!=0)
        {
          QTime t1;
          pInterface->stop();
          t1=QTime::currentTime();
          for (;;)
          {
            int tDiff;
            usleep(1000);
            if (pInterface->status()==0) break;
            tDiff=t1.msecsTo(QTime::currentTime());
            if (tDiff<0) tDiff+=86400000;
            if (tDiff>1000) break;
          }
        }
        if (iDevice<ARRAY_SIZE(g_asDevices))
          MSG_DEBUG << "stop device " << g_asDevices[iDevice];
        else
          MSG_DEBUG << "stop device " << iDevice;
      }
    }
    module_status=DONE;
    return CARESS::OK;
  }
  catch (...)
  {
    MSG_ERROR << "stop - Caught unknown exception.";
    return CARESS::NOT_OK;
  }
}

/*!
  \brief drive device / new set point
  \param[in]     kind                kind of drive
  \param[in]     id                  CARESS id
  \param[in]     data                new set point
  \param[in,out] calculated_timeout  add device timeout
  \param[out]    delay               delay device and try again
  \param[out]    module_status       current device status
  \note this function call is ignored
 */
CARESS::ReturnType CORBADevice_i::drive_module(CORBA::Long kind,
                                               CORBA::Long id,
                                               const CARESS::Value& data,
                                               CORBA::Long& calculated_timeout,
                                               CORBA::Boolean& delay,
                                               CORBA::Long& module_status)
{
  QMutexLocker lock(&m_mutex);
  (void) kind;
  (void) id;
  (void) data;
  (void) calculated_timeout;

  strcpy(m_szErrorMessage,"drive is not implemented.");
  MSG_ERROR << "drive - Not implemented.";
  delay=0;
  module_status=OFF_LINE;
  return CARESS::NOT_OK;
}

/*!
  \brief handle counters: clear, new count target, master/slave, etc.
  \param[in]     kind                kind of load
  \param[in]     id                  CARESS id
  \param[in]     data                new counter value/target
  \param[out]    module_status       current device status
 */
CARESS::ReturnType CORBADevice_i::load_module(CORBA::Long kind,
                                              CORBA::Long id,
                                              const CARESS::Value& data,
                                              CORBA::Long& module_status)
{
  QMutexLocker lock(&m_mutex);
  MSG_DEBUG << "load(kind=" << kind << ", id=" << id << ')';
  m_szErrorMessage[0]='\0';
  try
  {
    QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
    int iDevice;

    if (!pInterface)
    {
      strcpy(m_szErrorMessage,"control interface not initialized");
      MSG_DEBUG << "load - " << (const char*)m_szErrorMessage;
      module_status=OFF_LINE;
      return CARESS::NOT_OK;
    }

    for (iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
      if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
        break;

    if (iDevice<0)
    {
      strcpy(m_szErrorMessage,"invalid device");
      MSG_DEBUG << "load - " << (const char*)m_szErrorMessage;
      module_status=OFF_LINE;
      return CARESS::NOT_OK;
    }

    switch (kind)
    {
      case 14: // LOADMASTER
      {
        int iTmpDev=0;
        double dblTarget=0.0;
        bool bOK=false;

        m_iMaster=-1;
        switch (iDevice)
        {
          case QMESYDAQ_MON1:  iTmpDev=M1CT; break;
          case QMESYDAQ_MON2:  iTmpDev=M2CT; break;
          case QMESYDAQ_MON3:  iTmpDev=M3CT; break;
          case QMESYDAQ_MON4:  iTmpDev=M4CT; break;
          case QMESYDAQ_TIMER: iTmpDev=TCT;  break;
          default: // QMESYDAQ_EVENT, QMESYDAQ_HISTOGRAM, QMESYDAQ_DIFFRACTOGRAM, QMESYDAQ_SPECTROGRAM:
            iDevice=QMESYDAQ_EVENT;
            iTmpDev=EVCT;
            break;
        }

        switch (data._d())
        {
          case CARESS::TypeLong:   dblTarget=data.l();   bOK=true; break;
          case CARESS::TypeLong64: dblTarget=data.l64(); bOK=true; break;
          case CARESS::TypeFloat:  dblTarget=data.f();   bOK=true; break;
          case CARESS::TypeDouble: dblTarget=data.d();   bOK=true; break;
          case CARESS::TypeArrayByte:
            if (data.ab().length()>0) { dblTarget=data.ab()[0]; bOK=true; }
            else strcpy(m_szErrorMessage,"got no data");
            break;
          case CARESS::TypeArrayLong:
            if (data.al().length()>0) { dblTarget=data.al()[0]; bOK=true; }
            else strcpy(m_szErrorMessage,"got no data");
            break;
          case CARESS::TypeArrayLong64:
            if (data.al64().length()>0) { dblTarget=data.al64()[0]; bOK=true; }
            else strcpy(m_szErrorMessage,"got no data");
            break;
          case CARESS::TypeArrayFloat:
            if (data.af().length()>0) { dblTarget=data.af()[0]; bOK=true; }
            else strcpy(m_szErrorMessage,"got no data");
            break;
          case CARESS::TypeArrayDouble:
            if (data.ad().length()>0) { dblTarget=data.ad()[0]; bOK=true; }
            else strcpy(m_szErrorMessage,"got no data");
            break;
          default: strcpy(m_szErrorMessage,"invalid data type"); break;
        }

        if (bOK)
        {
          m_iMaster=iDevice;
          if (iDevice==QMESYDAQ_TIMER) dblTarget/=m_dblTimerScale;
          pInterface->selectCounter(iTmpDev);
          pInterface->setPreSelection(dblTarget);
          if (iDevice<ARRAY_SIZE(g_asDevices))
            MSG_DEBUG << "master device " << g_asDevices[iDevice];
          else
            MSG_DEBUG << "master device " << iDevice;
        }
        else
        {
          MSG_DEBUG << "load - " << (const char*)m_szErrorMessage;
          module_status=LOADED;
          return CARESS::NOT_OK;
        }
        break;
      }
      case 15: // LOADSLAVE
        if (m_iMaster==iDevice)
          m_iMaster=-1;
        if (iDevice<ARRAY_SIZE(g_asDevices))
          MSG_DEBUG << "slave device " << g_asDevices[iDevice];
        else
          MSG_DEBUG << "slave device " << iDevice;
        break;
      case 16: // RESETMODULE
        pInterface->clear();
        if (iDevice<ARRAY_SIZE(g_asDevices))
          MSG_DEBUG << "clear device " << g_asDevices[iDevice];
        else
          MSG_DEBUG << "clear device " << iDevice;
        break;
      default: // ignore other load kinds
        break;
    }

    module_status=LOADED;
    return CARESS::OK;
  }
  catch (...)
  {
    strcpy(m_szErrorMessage,"catched unknown exception");
    MSG_DEBUG << "load - " << (const char*)m_szErrorMessage;
    module_status=OFF_LINE;
    return CARESS::NOT_OK;
  }
}

/*!
  \brief load additional information from CARESS

      this function is used for loading
      \li additional configuration (<tt>kind==0</tt>)
      \li additional CARESS commands (<tt>kind==5</tt>)
      \li content of other CARESS commands at start (<tt>kind==7</tt>)
      \li position other CARESS devices at start (<tt>kind==2</tt>)
      \li header and mapping for list mode file (<tt>kind==18</tt>)
  \param[in]     kind                kind of load block
  \param[in]     id                  CARESS id
  \param[in]     start_channel       target start channel (starts with 1, not really used)
  \param[in]     end_channel         target end channel (starts with 1)
  \param[out]    module_status       current device status
  \param[in]     data                load data
 */
CARESS::ReturnType CORBADevice_i::loadblock_module(CORBA::Long kind,
                                                   CORBA::Long id,
                                                   CORBA::Long start_channel,
                                                   CORBA::Long end_channel,
                                                   CORBA::Long& module_status,
                                                   const CARESS::Value& data)
{
  QMutexLocker lock(&m_mutex);
  int iDevice;
  MSG_DEBUG << "loadblock(kind=" << kind << ", id=" << id << ", start=" << start_channel << ", end=" << end_channel << ')';

  for (iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
    if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
      break;

  if (iDevice<0)
  {
    strcpy(m_szErrorMessage,"invalid device");
    MSG_DEBUG << "loadblock - " << (const char*)m_szErrorMessage;
    module_status=OFF_LINE;
    return CARESS::NOT_OK;
  }

  if (kind==0 && start_channel==1 && start_channel<end_channel &&
      data._d()==CARESS::TypeArrayByte &&
      (CORBA::ULong)end_channel==data.ab().length())
  {
    // load special configuration
    const CARESS::ArrayByte& ab=data.ab();
    const char* pStart=(const char*)(&ab[0]);
    const char* pEnd=pStart+end_channel;
    const char *p1, *p2, *p3;

    while (pStart<pEnd)
    {
      for (p1=p2=pStart; p2<pEnd && *p2!='\r' && *p2!='\n'; ++p2)
        if (*p2=='=' && p1==pStart)
          p1=p2+1;

      int iNameLen=p1-pStart-1;
      if (p1!=p2 && iNameLen==10 && strncasecmp(pStart,"CARESSInfo",10)==0)
      {
        // extract CARESS revision: CARESS r1537 and later is able to handle 64 bit IEEE numbers
        long lRevision=strtol(p1,(char**)&p3,10);
        if (p1<p3 && p3<p2 && (p3-p1)>=4 && lRevision>0 && lRevision<0x7FFFFFFF)
        {
          m_b64Bit[iDevice]=(lRevision>=1537);
          if (iDevice<ARRAY_SIZE(g_asDevices))
            MSG_DEBUG << "use " << ((int)m_b64Bit[iDevice]?64:32) << "bit data for device " << g_asDevices[iDevice];
          else
            MSG_DEBUG << "use " << ((int)m_b64Bit[iDevice]?64:32) << "bit data for device " << id;
        }
      }
      else if (p1!=p2 && iNameLen>8 && strncasecmp(pStart,"mesydaq_",8)==0)
      {
        // may be, we found an option to configure
        iNameLen-=8;
        pStart+=8;

        // forced use of 32 bit return values
        if ((iNameLen>=8 && strncasecmp(pStart,"return32",8)==0) ||
            (iNameLen>=5  && strncasecmp(pStart,"use32",5)==0) ||
            (iNameLen==5  && strncasecmp(pStart,"32bit",5)==0) ||
            (iNameLen>=6 && strncasecmp(pStart,"force32",6)==0))
        {
          int iValueLen=p2-p1;
          if ((iValueLen==3 && strncasecmp(p1,"yes"  ,3)==0) ||
              (iValueLen==2 && strncasecmp(p1,"on"   ,2)==0) ||
              (iValueLen==4 && strncasecmp(p1,"true" ,4)==0)) m_b64Bit[iDevice]=false;
          else if ((iValueLen==2 && strncasecmp(p1,"no"   ,2)==0) ||
                   (iValueLen==3 && strncasecmp(p1,"off"  ,3)==0) ||
                   (iValueLen==5 && strncasecmp(p1,"false",5)==0)) m_b64Bit[iDevice]=true;
          if (iDevice<ARRAY_SIZE(g_asDevices))
            MSG_DEBUG << "force " << ((int)m_b64Bit[iDevice]?64:32) << "bit data for device " << g_asDevices[iDevice];
          else
            MSG_DEBUG << "force " << ((int)m_b64Bit[iDevice]?64:32) << "bit data for device " << id;
        } else
          // forced use of 64 bit return values
          if ((iNameLen>=8 && strncasecmp(pStart,"return64",8)==0) ||
              (iNameLen>=5  && strncasecmp(pStart,"use64",5)==0) ||
              (iNameLen==5  && strncasecmp(pStart,"64bit",5)==0) ||
              (iNameLen>=6 && strncasecmp(pStart,"force64",6)==0))
          {
            int iValueLen=p2-p1;
            if ((iValueLen==3 && strncasecmp(p1,"yes"  ,3)==0) ||
                (iValueLen==2 && strncasecmp(p1,"on"   ,2)==0) ||
                (iValueLen==4 && strncasecmp(p1,"true" ,4)==0)) m_b64Bit[iDevice]=true;
            else if ((iValueLen==2 && strncasecmp(p1,"no"   ,2)==0) ||
                     (iValueLen==3 && strncasecmp(p1,"off"  ,3)==0) ||
                     (iValueLen==5 && strncasecmp(p1,"false",5)==0)) m_b64Bit[iDevice]=false;
            if (iDevice<ARRAY_SIZE(g_asDevices))
              MSG_DEBUG << "force " << ((int)m_b64Bit[iDevice]?64:32) << "bit data for device " << g_asDevices[iDevice];
            else
              MSG_DEBUG << "force " << ((int)m_b64Bit[iDevice]?64:32) << "bit data for device " << id;
          } else
            // timer scaler/factor
            if (iDevice==QMESYDAQ_TIMER &&
                ((iNameLen>=9 && strncasecmp(pStart,"timescale",9)==0) ||
                 (iNameLen=10 && strncasecmp(pStart,"timefactor",10)==0)))
            {
              m_dblTimerScale=QString::fromLatin1(p1,p2-p1).toDouble();
              if (m_dblTimerScale<=0.0) m_dblTimerScale=DEFAULTTIMEFACTOR;
              MSG_DEBUG << "load time scale/factor " << m_dblTimerScale;
            } else
              // list mode
              if ((iDevice==QMESYDAQ_HISTOGRAM || iDevice==QMESYDAQ_DIFFRACTOGRAM || iDevice==QMESYDAQ_SPECTROGRAM) &&
                  iNameLen==8 && strncasecmp(pStart,"listmode",8)==0)
              {
                int iValueLen=p2-p1;
                QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
                if ((iValueLen==3 && strncasecmp(p1,"yes"  ,3)==0) ||
                    (iValueLen==2 && strncasecmp(p1,"on"   ,2)==0) ||
                    (iValueLen==4 && strncasecmp(p1,"true" ,4)==0)) m_bListmode=true;
                else if ((iValueLen==2 && strncasecmp(p1,"no"   ,2)==0) ||
                         (iValueLen==3 && strncasecmp(p1,"off"  ,3)==0) ||
                         (iValueLen==5 && strncasecmp(p1,"false",5)==0)) m_bListmode=false;
                if (pInterface)
                  pInterface->setListMode(m_bListmode);
                MSG_DEBUG << "device " << g_asDevices[iDevice] << " - listmode=" << ((const char*)(m_bListmode?"on":"off"));
              } else
                // list file
                if ((iDevice==QMESYDAQ_HISTOGRAM || iDevice==QMESYDAQ_DIFFRACTOGRAM || iDevice==QMESYDAQ_SPECTROGRAM) &&
                    ((iNameLen==8 && strncasecmp(pStart,"listfile",8)==0) ||
                     (iNameLen==12 && strncasecmp(pStart,"listmodefile",12)==0)))
                {
                  QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
                  m_sListfile=QString::fromLatin1(p1,p2-p1);
                  m_bListmode=true;
                  if (pInterface)
                  {
                    pInterface->setListFileName(m_sListfile);
                    pInterface->setListMode(m_bListmode);
                  }
                  MSG_DEBUG << "device " << g_asDevices[iDevice] << " - listfile=" << m_sListfile.toLatin1().constData();
                }
      }
      pStart=p2+1;
    }
    module_status=LOADED;
    return CARESS::OK;
  }

  if (kind==5/*GENERATION*/ && start_channel==1 && start_channel<end_channel &&
      data._d()==CARESS::TypeArrayByte && (CORBA::ULong)end_channel==data.ab().length())
  {
    // CARESS commands "LOADTEXT" or "LOADFILE"
    const CARESS::ArrayByte& ab=data.ab();
    const char* pStart=(const char*)(&ab[0]);
    const char* pEnd=pStart+end_channel;
    QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());

    if (!pInterface)
    {
      strcpy(m_szErrorMessage,"control interface not initialized");
      MSG_DEBUG << "loadblock - " << (const char*)m_szErrorMessage;
      module_status=LOADED;
      return CARESS::NOT_OK;
    }

    while (pStart<pEnd && (pStart[0]==' ' || pStart[0]=='\t')) ++pStart;
    if ((iDevice==QMESYDAQ_HISTOGRAM || iDevice==QMESYDAQ_DIFFRACTOGRAM || iDevice==QMESYDAQ_SPECTROGRAM) &&
        (pStart+8)<pEnd && strncasecmp(pStart,"listmode",8)==0)
    {
      pStart+=8;
      while (pStart<pEnd && (pStart[0]==' ' || pStart[0]=='\t')) ++pStart;
      if (pStart<pEnd && pStart[0]=='=') ++pStart;
      while (pStart<pEnd && (pStart[0]==' ' || pStart[0]=='\t')) ++pStart;

      if ((pStart+2)<pEnd && strncasecmp(pStart,"on",2)==0) m_bListmode=true;
      else if ((pStart+3)<pEnd && strncasecmp(pStart,"off",3)==0) m_bListmode=false;
      m_sListfile="";
      pInterface->setListFileName(m_sListfile);
      pInterface->setListMode(m_bListmode);
    }
    else if ((iDevice==QMESYDAQ_HISTOGRAM || iDevice==QMESYDAQ_DIFFRACTOGRAM || iDevice==QMESYDAQ_SPECTROGRAM) &&
             (pStart+8)<pEnd && strncasecmp(pStart,"listfile",8)==0)
    {
      pStart+=8;
      while (pStart<pEnd && (pStart[0]==' ' || pStart[0]=='\t')) ++pStart;
      if (pStart<pEnd && pStart[0]=='=') ++pStart;
      while (pStart<pEnd && (pStart[0]==' ' || pStart[0]=='\t')) ++pStart;

      m_bListmode=true;
      m_sListfile=QString::fromLatin1(pStart,pEnd-pStart);
      pInterface->setListFileName(m_sListfile);
      pInterface->setListMode(m_bListmode);
    }
    else
    {
      pStart=(const char*)(&ab[0]);
      if (iDevice<ARRAY_SIZE(g_asDevices))
        MSG_DEBUG << "loadblock(loadtext device " << g_asDevices[iDevice] << ") - unknown command '" << QByteArray::fromRawData(pStart,pEnd-pStart).constData() << '\'';
      else
        MSG_DEBUG << "loadblock(loadtext device " << id << ") - unknown command '" << QByteArray::fromRawData(pStart,pEnd-pStart).constData() << '\'';
    }

    // TODO: test for additional user commands from CARESS if meaningful
    module_status=LOADED;
    return CARESS::OK;
  }

  if (kind==7/*SETACTION*/ && start_channel==1 && start_channel<end_channel &&
      data._d()==CARESS::TypeArrayByte && (CORBA::ULong)end_channel==data.ab().length())
  {
    // CARESS configuration option "startcommands"
    // data is simple text with unix-end-of-line in variable data.ab()[]
    const char* pStart=(const char*)(&data.ab()[0]);
    CORBA::ULong uLen=data.ab().length();
    if (iDevice<ARRAY_SIZE(g_asDevices))
      MSG_DEBUG << "loadblock(startcommands device " << g_asDevices[iDevice] << ") - unknown command '" << QByteArray::fromRawData(pStart,uLen).constData() << '\'';
    else
      MSG_DEBUG << "loadblock(startcommands device " << id << ") - unknown command '" << QByteArray::fromRawData(pStart,uLen).constData() << '\'';

    // TODO: CARESS is able to load selected command structures before any measurment step
    module_status=LOADED;
    return CARESS::OK;
  }

  if (kind==2/*LOADACTION*/ && start_channel==1 && start_channel<end_channel &&
      data._d()==CARESS::TypeArrayByte && (CORBA::ULong)end_channel==data.ab().length())
  {
    // CARESS configuration option "startvalues"
    // data is simple text with unix-end-of-line in variable data.ab()[]
    const char* pStart=(const char*)(&data.ab()[0]);
    CORBA::ULong uLen=data.ab().length();
    if (iDevice<ARRAY_SIZE(g_asDevices))
      MSG_DEBUG << "loadblock(startvalues device " << g_asDevices[iDevice] << ") - unknown command '" << QByteArray::fromRawData(pStart,uLen).constData() << '\'';
    else
      MSG_DEBUG << "loadblock(startvalues device " << id << ") - unknown command '" << QByteArray::fromRawData(pStart,uLen).constData() << '\'';

    // TODO: CARESS is able to load current positions of selected devices before any measurment step
    module_status=LOADED;
    return CARESS::OK;
  }

  if (kind==18/*SPECIALLOAD*/ && start_channel==1 && start_channel<end_channel &&
      data._d()==CARESS::TypeArrayByte && (CORBA::ULong)end_channel==data.ab().length())
  {
    // load special mapping and correction data
    QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
    quint16 w=(quint16)m_lSourceChannels,h=960;
    const unsigned char* pData=&data.ab()[0];
    CORBA::ULong uLength=data.ab().length();

    if (!pInterface)
    {
      strcpy(m_szErrorMessage,"control interface not initialized");
      MSG_DEBUG << "loadblock - " << (const char*)m_szErrorMessage;
      module_status=LOADED;
      return CARESS::NOT_OK;
    }

    if (iDevice<ARRAY_SIZE(g_asDevices))
      MSG_DEBUG << "loadblock(binary device " << g_asDevices[iDevice] << ") - " << uLength << " bytes of binary data";
    else
      MSG_DEBUG << "loadblock(binary device " << id << ") - " << uLength << " bytes of binary data";

    if (m_lSourceChannels<=0 || m_lSourceChannels>=65536)
    {
      bool bListMode=pInterface->getListMode();
      quint32 uRun=pInterface->getRunID();
      if (bListMode)
        pInterface->setListMode(false);
      pInterface->setRunID(uRun,false);
      pInterface->start();
      sleep(1);
      pInterface->stop();
      pInterface->setRunID(uRun,false);
      pInterface->readHistogramSize(w,h);
      if (bListMode)
        pInterface->setListMode(bListMode);
      m_lSourceChannels=w;
    }

    pInterface->setMappingCorrection(parseCaressMapCorrection((const char*) \
                                                              pData,uLength,w>m_lHistogramX?w:m_lHistogramX,h,m_lHistogramX,m_lHistogramY));
    pInterface->setListFileHeader(pData,(int)uLength);
    module_status=LOADED;
    return CARESS::OK;
  }

  strcpy(m_szErrorMessage,"loadblock: Not implemented kind.");
  if (iDevice<ARRAY_SIZE(g_asDevices))
    MSG_DEBUG << "loadblock(device " << g_asDevices[iDevice] << ") - " << (const char*)m_szErrorMessage;
  else
    MSG_DEBUG << "loadblock(device " << id << ") - " << (const char*)m_szErrorMessage;
  module_status=LOADED;
  return CARESS::NOT_OK;
}

/*!
  \brief read device value and status (histogram: read sum only)
  \param[in]     kind                kind of load block
  \param[in]     id                  CARESS id
  \param[out]    module_status       current device status
  \param[out]    data                current value
 */
CARESS::ReturnType CORBADevice_i::read_module(CORBA::Long kind,
                                              CORBA::Long id,
                                              CORBA::Long& module_status,
                                              CARESS::Value_out data)
{
  QMutexLocker lock(&m_mutex);
  (void)kind;
  CARESS::Value_var val=new CARESS::Value;
  CARESS::ReturnType result=CARESS::NOT_OK;

  m_szErrorMessage[0]='\0';
  try
  {
    QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
    int iDevice;
    double dblValue;

    if (!pInterface)
    {
      strcpy(m_szErrorMessage,"control interface not initialized");
      MSG_DEBUG << "read - " << (const char*)m_szErrorMessage;
      val->l(0);
      data=val._retn();
      module_status=OFF_LINE;
      return CARESS::NOT_OK;
    }

    for (iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
      if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
        break;

    if (iDevice<0)
    {
      strcpy(m_szErrorMessage,"invalid device");
      MSG_DEBUG << "read - " << (const char*)m_szErrorMessage;
      val->l(0);
      data=val._retn();
      module_status=OFF_LINE;
      return CARESS::NOT_OK;
    }

    switch (iDevice)
    {
      case QMESYDAQ_MON1: dblValue=pInterface->readCounter(M1CT); break;
      case QMESYDAQ_MON2: dblValue=pInterface->readCounter(M2CT); break;
      case QMESYDAQ_MON3: dblValue=pInterface->readCounter(M3CT); break;
      case QMESYDAQ_MON4: dblValue=pInterface->readCounter(M4CT); break;
      case QMESYDAQ_TIMER: dblValue=m_dblTimerScale*pInterface->readCounter(TCT); break;
      default: // QMESYDAQ_EVENT, QMESYDAQ_HISTOGRAM, QMESYDAQ_DIFFRACTOGRAM, QMESYDAQ_SPECTROGRAM
        dblValue=pInterface->readCounter(EVCT); break;
    }
    if (m_b64Bit[iDevice])
      val->l64((CORBA::LongLong)dblValue);
    else
      val->l((CORBA::Long)dblValue);
    module_status=pInterface->status() ? ACTIVE : DONE;
    result=CARESS::OK;
  }
  catch (...)
  {
    MSG_ERROR << "read - catched unknown exception";
    val->l(0);
    module_status=NOT_ACTIVE;
  }
  data=val._retn();
  return result;
}

/*!
  \brief prepare histogram readout or read special device data
  \param[in]     kind                kind of read block
  \param[in]     id                  CARESS id
  \param[in,out] start_channel       start channel (starts with 1)
  \param[in,out] end_channel         end channel (starts with 1)
  \param[in,out] type                current device status
 */
/***************************************************************************
 * prepare histogram readout or read special device data
 ***************************************************************************/
CARESS::ReturnType CORBADevice_i::readblock_params(CORBA::Long kind,
                                                   CORBA::Long id,
                                                   CORBA::Long& start_channel,
                                                   CORBA::Long& end_channel,
                                                   CARESS::DataType& type)
{
  QMutexLocker lock(&m_mutex);
  (void)kind;

  //MSG_DEBUG << "readblock_params(kind=" << kind << ", id=" << id << ", start_channel=" << start_channel << ", end_channel=" << end_channel << ", type=long)";
  m_szErrorMessage[0]='\0';
  try
  {
    QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
    int iDevice;

    type=CARESS::TypeLong;
    start_channel=1;
    end_channel=1;
    if (!pInterface)
    {
      strcpy(m_szErrorMessage,"control interface not initialized");
      MSG_DEBUG << "readblock_params - " << (const char*)m_szErrorMessage;
      return CARESS::NOT_OK;
    }

    for (iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
      if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
        break;

    if (iDevice<0)
    {
      strcpy(m_szErrorMessage,"invalid device");
      MSG_DEBUG << "readblock_params - " << (const char*)m_szErrorMessage;
      return CARESS::NOT_OK;
    }

    switch (iDevice)
    {
      case QMESYDAQ_HISTOGRAM:
      {
        quint16 w=0,h=0;
        pInterface->readHistogramSize(w,h);
        if (m_lHistogramX==0 && m_lHistogramY==0)
          end_channel=((CORBA::Long)w)*((CORBA::Long)h);
        else
          end_channel=m_lHistogramX*m_lHistogramY;
        m_aullDetectorData=pInterface->readHistogram();
        m_iDetectorWidth=w;
        if (m_iDetectorWidth<1)
          m_iDetectorWidth=1;
#ifdef DEBUGBUILD
        do
        {
          bool bPrintAny=false;
          MSG_DEBUG << "read histogram: width=" << m_iDetectorWidth << " count=" << m_aullDetectorData.count();
          int iDetectorHeight=m_aullDetectorData.count()/m_iDetectorWidth;
          for (int y=0; y<iDetectorHeight; ++y)
          {
            QString line;
            char buffer[16];
            int iStart=m_iDetectorWidth*y;
            bool bPrint=false;
            snprintf(buffer,ARRAY_SIZE(buffer),"%03d ",y);
            buffer[ARRAY_SIZE(buffer)-1]='\0';
            line+=QString::fromLatin1(buffer);
            for (int x=0; x<m_iDetectorWidth; ++x)
            {
              quint64 z=m_aullDetectorData.value(iStart+x);
              if (z!=0) bPrint=true;
              snprintf(buffer,ARRAY_SIZE(buffer)," %5Ld",z);
              buffer[ARRAY_SIZE(buffer)-1]='\0';
              line+=QString::fromLatin1(buffer);
            }
            if (bPrint)
            {
              bPrintAny=true;
              MSG_DEBUG << line.toLocal8Bit().constData();
            }
          }
          if (!bPrintAny)
            MSG_DEBUG << "all values are zero";
        } while (0);
#endif
        break;
      }
      case QMESYDAQ_DIFFRACTOGRAM:
#ifdef DEBUGBUILD
        Q_ASSERT(m_iDetectorWidth>0);
#endif
        end_channel=m_iDetectorWidth=m_lDiffractogramWidth;
        m_aullDetectorData=pInterface->readDiffractogram();
        MSG_DEBUG << "read diffractogram: width=" << m_iDetectorWidth << " count=" << m_aullDetectorData.count();
        break;
      case QMESYDAQ_SPECTROGRAM:
#ifdef DEBUGBUILD
        Q_ASSERT(m_iDetectorWidth>0);
#endif
        end_channel=m_iDetectorWidth=m_lSpectrogramWidth;
        m_aullDetectorData=pInterface->readSpectrogram(m_lSpectrogramChannel);
        MSG_DEBUG << "read spectrogram: width=" << m_iDetectorWidth << " count=" << m_aullDetectorData.count();
        break;
      default:
        end_channel=1;
        MSG_DEBUG << "read other";
        break;
    }
    type=m_b64Bit[iDevice] ? CARESS::TypeLong64 : CARESS::TypeLong;
    return CARESS::OK;
  }
  catch (...)
  {
    strcpy(m_szErrorMessage,"catched unknown exception");
    MSG_DEBUG << "readblock_params - " << (const char*)m_szErrorMessage;
    return CARESS::NOT_OK;
  }
}

/***************************************************************************
 * histogram readout or read special device data
 ***************************************************************************/
static void readblock_module_helper(QList<quint64> src, quint16 srcwidth,
                                    QList<quint64>& dst, quint32 dstwidth);
/*!
  \brief histogram readout or read special device data
  \param[in]   kind                kind of read block
  \param[in]   id                  CARESS id
  \param[in]   start_channel       start channel (starts with 1)
  \param[in]   end_channel         end channel (starts with 1)
  \param[out]  module_status       current device status
  \param[out]  data                array of values in selected range
 */
CARESS::ReturnType CORBADevice_i::readblock_module(CORBA::Long kind,
                                                   CORBA::Long id,
                                                   CORBA::Long start_channel,
                                                   CORBA::Long end_channel,
                                                   CORBA::Long& module_status,
                                                   CARESS::Value_out data)
{
  QMutexLocker lock(&m_mutex);
  (void)kind;

  CARESS::Value_var val=new CARESS::Value;
  CARESS::ReturnType result=CARESS::NOT_OK;

  try
  {
    QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
    CARESS::ArrayLong64_var al64=new CARESS::ArrayLong64;
    int iDevice;

    if (!pInterface)
    {
      strcpy(m_szErrorMessage,"control interface not initialized");
      MSG_DEBUG << "readblock - " << (const char*)m_szErrorMessage;
      val->l(0);
      data=val._retn();
      return CARESS::NOT_OK;
    }

    for (iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
      if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
        break;

    if (iDevice<0)
    {
      strcpy(m_szErrorMessage,"invalid device");
      MSG_DEBUG << "readblock - " << (const char*)m_szErrorMessage;
      val->l(0);
      data=val._retn();
      return CARESS::NOT_OK;
    }

    if (start_channel<1) start_channel=1;
#ifdef DEBUGBUILD
    Q_ASSERT(m_iDetectorWidth>0);
#endif
    switch (iDevice)
    {
      case QMESYDAQ_HISTOGRAM:
      {
        QList<quint64> tmpsrc,tmpdst,dsthistogram;
        CORBA::Long x,y,lHistoX,lHistoY;
        int iDetectorHeight=m_aullDetectorData.count()/m_iDetectorWidth;

        lHistoX=m_lHistogramX;
        lHistoY=m_lHistogramY;
        if (lHistoX==0 && lHistoY)
        {
          lHistoX=m_iDetectorWidth;
          lHistoY=iDetectorHeight;
        }
        if (lHistoX<1 || lHistoY<1)
        {
          strcpy(m_szErrorMessage,"invalid histogram size");
          MSG_DEBUG << "readblock - " << (const char*)m_szErrorMessage;
          val->l(0);
          data=val._retn();
          return CARESS::NOT_OK;
        }
        if (end_channel>lHistoX*lHistoY)
          end_channel=lHistoX*lHistoY;

        // scale histogram to given size
        if (iDetectorHeight<lHistoY)
        {
          // source is smaller height (grow)
          for (y=0; y<iDetectorHeight; ++y)
          {
            int iStartY=(int)((((double)y)*lHistoY)/iDetectorHeight);
            int iEndY=(int)((((double)y+1.0)*lHistoY)/iDetectorHeight);
            int iStart=m_iDetectorWidth*y;
            tmpsrc.clear();
            tmpdst.clear();
            for (x=0; x<m_iDetectorWidth; ++x)
            {
              if (x<m_aullDetectorData.count())
                tmpsrc.append(m_aullDetectorData.value(iStart+x));
              else
                tmpsrc.append(0ULL);
            }
            readblock_module_helper(tmpsrc,m_iDetectorWidth,tmpdst,lHistoX);
            while (iStartY++ < iEndY)
              dsthistogram.append(tmpdst);
          }
        }
        else
        {
          // source is greater or equal height (shrink)
          for (y=0; y<lHistoY; ++y)
          {
            int iStartY=(int)((((double)y)*iDetectorHeight)/lHistoY);
            int iEndY=(int)((((double)y+1.0)*iDetectorHeight)/lHistoY);
            tmpdst.clear();
            while (iStartY<iEndY)
            {
              int iStart=m_iDetectorWidth * iStartY++;
              tmpsrc.clear();
              for (x=0; x<m_iDetectorWidth; ++x)
              {
                if (x<m_aullDetectorData.count())
                  tmpsrc.append(m_aullDetectorData.value(iStart+x));
                else
                  tmpsrc.append(0ULL);
              }
              readblock_module_helper(tmpsrc,m_iDetectorWidth,tmpdst,lHistoX);
            }
            dsthistogram.append(tmpdst);
          }
        }

#ifdef DEBUGBUILD
        do
        {
          bool bPrintAny=false;
          MSG_DEBUG << "read histogram: width=" << lHistoX << " height=" << lHistoY;
          Q_ASSERT((lHistoX*lHistoY)==dsthistogram.count());
          for (int y=0; y<lHistoY; ++y)
          {
            QString line;
            char buffer[16];
            int iStart=lHistoX*y;
            bool bPrint=false;
            snprintf(buffer,ARRAY_SIZE(buffer),"%03d ",y);
            buffer[ARRAY_SIZE(buffer)-1]='\0';
            line+=QString::fromLatin1(buffer);
            for (int x=0; x<lHistoX; ++x)
            {
              quint64 z=dsthistogram.value(iStart+x);
              if (z!=0) bPrint=true;
              snprintf(buffer,ARRAY_SIZE(buffer)," %5Ld",z);
              buffer[ARRAY_SIZE(buffer)-1]='\0';
              line+=QString::fromLatin1(buffer);
            }
            if (bPrint)
            {
              bPrintAny=true;
              MSG_DEBUG << "%s",line.toLocal8Bit().constData();
            }
          }
          if (!bPrintAny)
            MSG_DEBUG << "all values are zero";
        } while (0);
#endif
        y=end_channel-start_channel+1;
        al64->length(y);
        for (x=0; x<y; ++x)
          al64[x]=dsthistogram.value(x+start_channel-1);
        break;
      }

      case QMESYDAQ_DIFFRACTOGRAM:
      case QMESYDAQ_SPECTROGRAM:
      {
        QList<quint64> dst;
        int i,j;
        i=(iDevice==QMESYDAQ_DIFFRACTOGRAM) ? m_lDiffractogramWidth : m_lSpectrogramWidth;
        if (i>0)
          readblock_module_helper(m_aullDetectorData,m_aullDetectorData.count(),dst,i);
        else
          dst=m_aullDetectorData;
        if (end_channel>(int)dst.count())
          end_channel=dst.size();
        al64->length(dst.size());
        j=end_channel-start_channel+1;
        for (i=0; i<j; ++i)
          al64[i]=dst.value(i+start_channel-1);
        break;
      }

      default:
        al64->length(0);
        break;
    }
    if (m_b64Bit[iDevice])
      val->al64(al64);
    else
    {
      CARESS::ArrayLong_var t2=new CARESS::ArrayLong;
      int i,j=al64->length();
      t2->length(j);
      for (i=0; i<j; ++i) t2[i]=al64[i];
      val->al(t2);
    }
    module_status=pInterface->status() ? ACTIVE : DONE;
    result=CARESS::OK;
  }
  catch (...)
  {
    strcpy(m_szErrorMessage,"Caught unknown exception");
    MSG_ERROR << "stop - " << (const char*)m_szErrorMessage << '.';
    module_status=NOT_ACTIVE;
    val->l((long)0);
  }
  data=val._retn();
  //MSG_DEBUG << "readblock_module(module_status=" << module_status << ")=" << (const char*)((result==CARESS::OK)?"OK":"NOT_OK");
  return result;
}

/*!
  \brief helper function to merge detector data

  grow, copy or shrink a single line of a histogram/diffractogram/spectrogram
  and merge it with a previous line
  \param[in]     src       source histogram line
  \param[in]     srcwidth  source histogram width
  \param[in,out] dst       mapped histogram line
  \param[in]     dstwidth  mapped histogram width
 */
static void readblock_module_helper(QList<quint64> src, quint16 srcwidth,
                                    QList<quint64>& dst, quint32 dstwidth)
{
  // scale data line to given size
  if (srcwidth<dstwidth)
  {
    // source is smaller width (grow)
    for (quint16 x=0; x<srcwidth; ++x)
    {
      int iStartX=(int)((((double)x)*dstwidth)/srcwidth);
      int iEndX=(int)((((double)x+1.0)*dstwidth)/srcwidth);
      int iSize=iEndX-iStartX;
      quint64 ullValue=src.value(x);
      quint64 ullMissing=ullValue;
      ullValue/=iSize;
      ullMissing-=iSize*ullValue;
      // spread source values to destination line (source sum of line and destination sum of line are equal)
      for (; iStartX<iEndX; ++iStartX)
      {
        quint64 ull=ullValue;
        if (ullMissing>0)
        {
          ++ull;
          --ullMissing;
        }
        if (iStartX<dst.size())
          dst[iStartX]+=ull;
        else
          dst.append(ull);
      }
    }
  }
  else
  {
    // source is greater or equal width (shrink)
    for (quint32 x=0; x<dstwidth; ++x)
    {
      int iStartX=(int)((((double)x)*srcwidth)/dstwidth);
      int iEndX=(int)((((double)x+1.0)*srcwidth)/dstwidth);
      quint64 ullCount=0;
      while (iStartX<iEndX)
        ullCount+=src.value(iStartX++);
      if ((int)x<dst.size())
        dst[x]+=ullCount;
      else
        dst.append(ullCount);
    }
  }
}

//! \brief device property: devices should be readable
CORBA::Boolean CORBADevice_i::is_readable_module(CORBA::Long id)
{
  QMutexLocker lock(&m_mutex);
  for (int iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
  {
    if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
    {
      MSG_DEBUG << "is_readable_module=TRUE";
      return 1;
    }
  }
  MSG_DEBUG << "is_readable_module - unknown device";
  return 0;
}

//! \brief device property: this device is cannot be driven
CORBA::Boolean CORBADevice_i::is_drivable_module(CORBA::Long id)
{
  QMutexLocker lock(&m_mutex);
  for (int iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
  {
    if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
    {
      MSG_DEBUG << "is_drivable_module=FALSE";
      return 0;
    }
  }
  MSG_DEBUG << "is_drivable_module - unknown device";
  return 0;
}

//! \brief device property: this device is counting
CORBA::Boolean CORBADevice_i::is_counting_module(CORBA::Long id)
{
  QMutexLocker lock(&m_mutex);
  for (int iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
  {
    if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
    {
      MSG_DEBUG << "is_counting_module=TRUE";
      return 1;
    }
  }
  MSG_DEBUG << "is_counting_module - unknown device";
  return 0;
}

//! \brief device property: this device is returns no digital I/O
CORBA::Boolean CORBADevice_i::is_status_module(CORBA::Long id)
{
  QMutexLocker lock(&m_mutex);
  for (int iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
  {
    if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
    {
      MSG_DEBUG << "is_status_module=FALSE";
      return 0;
    }
  }
  MSG_DEBUG << "is_status_module - unknown device";
  return 0;
}

//! \brief device property: this device needs no reference (incremental encoder do)
CORBA::Boolean CORBADevice_i::needs_reference_module(CORBA::Long id)
{
  QMutexLocker lock(&m_mutex);
  for (int iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
  {
    if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
    {
      MSG_DEBUG << "needs_reference_module=FALSE";
      return 0;
    }
  }
  MSG_DEBUG << "needs_reference_module - unknown device";
  return 0;
}

/*!
  \brief read extended device property

  supported attributes are
  \li detector_channels: return size (width and possibly height) of device
  \li error_description: last detailed textual error message
  \param[in] id   CARESS id
  \param[in] name name of attribute
  \return data or exception CARESS::ErrorDescription("not implemented")
 */
CARESS::Value* CORBADevice_i::get_attribute(CORBA::Long id, const char* name)
{
  QMutexLocker lock(&m_mutex);
  int iDevice;

  MSG_DEBUG << "get_attribute(id=" << id << ", name=" << name << ')';

  for (iDevice=QMESYDAQ_MAXDEVICES-1; iDevice>=0; --iDevice)
    if (m_lId[iDevice]>0 && m_lId[iDevice]==id)
      break;

  if (strcmp(name,"detector_channels")==0)
  {
    switch (iDevice)
    {
      case QMESYDAQ_HISTOGRAM:
      {
        CARESS::Value_var v=new CARESS::Value;
        CARESS::ArrayLong_var a=new CARESS::ArrayLong;
        CORBA::Long x=m_lHistogramX,y=m_lHistogramY;
        a->length(2);
        if (x==0 && y==0)
        {
          QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
          quint16 w,h;
          if (!pInterface)
            throw CARESS::ErrorDescription("control interface not initialized");
          pInterface->readHistogramSize(w,h);
          x=w;
          y=h;
        }
        a[0]=x;
        a[1]=y;
        v->al(a);
        return v._retn();
      }
      case QMESYDAQ_DIFFRACTOGRAM:
      case QMESYDAQ_SPECTROGRAM:
      {
        CARESS::Value_var v=new CARESS::Value;
        CARESS::ArrayLong_var a=new CARESS::ArrayLong;
        CORBA::Long x;
        if (iDevice==QMESYDAQ_DIFFRACTOGRAM)
          x=m_lDiffractogramWidth;
        else
          x=m_lSpectrogramWidth;
        a->length(1);
        if (x==0)
        {
          QMesyDAQDetectorInterface* pInterface=dynamic_cast<QMesyDAQDetectorInterface*>(m_theApp->getQtInterface());
          if (!pInterface)
            throw CARESS::ErrorDescription("control interface not initialized");
          QList<quint64> tmp=pInterface->readDiffractogram();
          x=tmp.count();
        }
        a[0]=x;
        v->al(a);
        return v._retn();
      }
    }
  }
  if (strcmp(name,"error_description")==0)
  {
    CARESS::Value_var v=new CARESS::Value;
    v->s((const char*)(&m_szErrorMessage[0]));
    return v._retn();
  }
  throw CARESS::ErrorDescription("not implemented");
}

/*!
  \brief write extended device property

  None of the attributes are writable. This functions generate an exception in any case.
  \param[in] id   CARESS id
  \param[in] name name of attribute
  \param[in] data new data for attribute
  \return exception CARESS::ErrorDescription("not implemented")
 */
/***************************************************************************
 * write extended device property
 ***************************************************************************/
void CORBADevice_i::set_attribute(CORBA::Long id, const char* name, const CARESS::Value& data)
{
  (void)id;
  (void)name;
  (void)data;
  MSG_ERROR << "set_attribute(id=" << id << ", name=" << name << ')';
  throw CARESS::ErrorDescription("not implemented");
}
