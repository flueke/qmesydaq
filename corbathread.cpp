/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>                       *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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

// CORBA (omniORB), generic CORBA device includes
//#include "corbadevice.h"
/*
#define NEED_GLOBAL_ORB
#include "corba_generic.h"

#include <omniORB4/IOP_S.h>
#include <omniORB4/IOP_C.h>
#include <omniORB4/callDescriptor.h>
#include <omniORB4/callHandle.h>
#include <omniORB4/objTracker.h>
*/
// application specific includes
#include "corbathread.h"
#include "mesydaq2.h"
#include "controlinterface.h"
#include "mdefines.h"
#include "measurement.h"
#include "mainwidget.h"

// standard includes
#include <stdio.h>
#include <stdlib.h>

// qt includes
#include <qdatetime.h>
#include <qthread.h>

// globals
//#define DEBUGOUTPUT
#undef OK
#undef NOT_OK

#define DAQSTART 1
#define DAQSTOP 2

const int CARESS_OK     = 0;
const int CARESS_NOT_OK = 1;

const int OFFLINE = 0;
const int ONLINE  = 1;

const int NOT_ACTIVE   = 1;
const int ACTIVE       = 2;
const int DONE         = 3;
const int LOADED       = 4;
const int ACTIVE1      = 5;
const int COMBO_ACTIVE = 6;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

/*
// class implementing IDL interface CORBADevice
class CORBADevice_i: public POA_CARESS::CORBADevice, public PortableServer::RefCountServantBase
{
private:
  // Make sure all instances are built on the heap by making the
  // destructor non-public
  //virtual ~CORBADevice_i();
public:
  // standard constructor
  CORBADevice_i(mesydaq2* tApp, CorbaThread* ct);
  virtual ~CORBADevice_i();

  // methods corresponding to defined IDL attributes and operations
  CARESS::ReturnType init_module(CORBA::Long kind, CORBA::Long id, const char* config_line, CORBA::Long& module_status);
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

protected:
  mesydaq2* m_pApp;
//  MesydaqDoc* m_pDoc;
//  Histogram*  m_pHistogram;
  CorbaThread* m_pThread;

  int m_iDevCount;
  struct mapping
  {
    CORBA::Long lCaressDevice;
    CORBA::Long lCaressState;
    bool        bCounter;  // counter: true                             | histogram: false
    bool        bMapData;  // counter: false=slave, true=master counter | histogram: false=listmode-off, true=listmode-ON
    CORBA::Long lMapData1; // counter: counter number                   | histogram: width
    CORBA::Long lMapData2; // counter: unused                           | histogram: height
  } m_aDevMap[256];

  static bool convertvalue(const CARESS::Value& data, CORBA::Long &lValue);
  bool storemapping(bool bCounter, CORBA::Long lID, CORBA::Long lData1, CORBA::Long lData2);
  struct mapping* findmapping(CORBA::Long lID);
};

static CORBA::Boolean bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_ptr objref, const char* szName);
*/
CorbaThread::CorbaThread()
	: m_pApp(NULL)
	, m_pCORBADevice(NULL)
	, terminate(false)
{
}

CorbaThread::~CorbaThread()
{
}

/** No descriptions */
void CorbaThread::run()
{
#if 0  
	while (m_pCORBADevicei != NULL)
	{ /* if OK then wait for ever */
		if (terminate == true)
		{
			qDebug("terminating");
			break;    
		}
		if (g_ORB->work_pending())
			g_ORB->perform_work();
		else
			usleep(10000);
	}
	m_pCORBADevice=NULL;
	g_ORB->destroy();

  /* otherwise program terminates */
//	qDebug("CORBA not running");
#endif
}

/** No descriptions */
bool CorbaThread::initializeCorba(MainWidget* App, ControlInterface* pcInt)
{
	qDebug("initialize corba");
	m_pApp = App;
	m_pInt = pcInt;
#if 0  
	if (m_pCORBADevice!=NULL)
		return false;

// Obtain a reference to the root POA.
	CORBA::Object_var obj = g_ORB->resolve_initial_references("RootPOA");
	PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);

// We allocate the objects on the heap.  Since these are reference
// counted objects, they will be deleted by the POA when they are no
// longer needed.
	m_pCORBADevice = new CORBADevice_i(App,this);
  
// Activate the objects.  This tells the POA that the objects are
// ready to accept requests.
	PortableServer::ObjectId_var myCORBADevice_iid = poa->activate_object(m_pCORBADevice);

// Obtain a reference to the object, and register it in
// the naming service.
	obj = m_pCORBADevice->_this();

// Obtain a reference to each object and output the stringified
// IOR to stdout
	do
	{
// IDL interface: CORBADevice
		CORBA::Object_var ref = m_pCORBADevice->_this();
		CORBA::String_var sior(g_ORB->object_to_string(ref));
//		printf("IDL object CORBADevice IOR = '%s'\n",(char*)sior); fflush(stdout);
	} while(0);

	if (!bindObjectToName(g_ORB, obj, "mesydaq"))
	{
		std::cerr << "cannot bind to naming service" << std::endl;
		return false;
	}
	m_pCORBADevice->_remove_ref();

// Obtain a POAManager, and tell the POA to start accepting
// requests on its objects.
	PortableServer::POAManager_var pman = poa->the_POAManager();
	pman->activate();
#endif
	return true;
}

/** stops and closes thread */
void CorbaThread::bye()
{
	qDebug("cthread: exit");
	terminate = true;
	qDebug("cthread: finished");
}

bool CorbaThread::asyncCmd(void)
{
  	
// return false if already a cmd is pending
	
	m_pInt->setCaressTaskPending(true);

// wait max. 100 ms. for completion
	quint8 waitCount(100);
  	for( ; m_pInt->isActive() && waitCount; --waitCount)
    		usleep(1000);

  	if(waitCount)
  		return true;
  	else
	{
  		qDebug("async cmd: failure");
  		return false;
	}
}
