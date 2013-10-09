#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "structures.h"
#include "mdefines.h"
#include <cmath>
#include <ctime>

//define PRINTPACKET 1

quint16 g_wMCPD8 = 2;
quint16 g_wSpectrumWidth = 64;
quint16 g_wSpectrumHeight = 960;
quint16 g_dwStopPacket = 0;
quint16 g_wTimerInterval = 20;
bool    g_bV4 = false;
#if defined(PRINTPACKET) && PRINTPACKET>0
int g_iPrintPacket=PRINTPACKET;
#else
#undef PRINTPACKET
#endif

//   ADET with
//   - one main hill and
//   - two small one's
//   near the lower border in the left half of the area
struct point {
  double x_min, x_max;	// 0 <= x_min < x_max <= 1
  double y_min, y_max;	// 0 <= y_min < y_max <= 1
  double x_exp, y_exp;	// exp < 0
  double height;		// height >= 2
} allPoints[] = {
  { 0.0, 1.0, 0.0, 1.0, -5.0, -5.0, 20.0},    // main
  { 0.0, 0.25, 0.0, 0.25, -9.0, -9.0, 15.0},   // 1st small
  { 0.25, 0.5, 0.0, 0.25, -9.0, -9.0, 15.0} }; // 2nd small

// V4 shape: 3 different lengths of detector tubes
// full length, 3/4 length and 1/2 length inside a round vacuum tube
struct scale {
  double src_start, src_end, intensity;
} v4Scale[] = {
  { 0.25,  0.75,  0.5  },
  { 0.125, 0.875, 0.75 },
  { 0.0,   1.0,   1.0  },
  { 0.0,   1.0,   1.0  },
  { 0.125, 0.875, 0.75 },
  { 0.25,  0.75,  0.5  } };

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_bDAQ(false),
  m_wRunId(1),
  m_qwMasterOffset(0ULL),
  m_dwPackets(0),
  m_qwLoopCount(0ULL)
{
  bool bMcpd=false;
  bool bWidth=false;
  bool bHeight=false;
  bool bStop=false;
  bool bTimer=false;
  bool bHelp=false;
  ui->setupUi(this);

  QStringList args=QApplication::instance()->arguments();
  for (int i=1; i<args.size(); ++i)
  {
    QString szArg(args[i]);
    if (szArg.length()<1) continue;
    if (szArg[0]!='-') continue;
    while (szArg[0]=='-') szArg.remove(0,1);
    if (szArg.indexOf("mcpd",Qt::CaseInsensitive)==0)
    {
      int j=szArg.indexOf('=')+1;
      if (j<2) continue;
      int l=atoi(szArg.toLatin1().constData()+j);
      if (l<1 || l>64)
        bHelp=true;
      else
      {
        bMcpd=true;
        g_wMCPD8=l;
      }
    }
    else if (szArg.indexOf("width",Qt::CaseInsensitive)==0)
    {
      int j=szArg.indexOf('=')+1;
      if (j<2) continue;
      int l=atoi(szArg.toLatin1().constData()+j);
      if (l<1 || l>64)
        bHelp=true;
      else
      {
        bWidth=true;
        g_wSpectrumWidth=l;
      }
    }
    else if (szArg.indexOf("height",Qt::CaseInsensitive)==0)
    {
      int j=szArg.indexOf('=')+1;
      if (j<2) continue;
      int l=atoi(szArg.toLatin1().constData()+j);
      if (l<1 || l>1024)
        bHelp=true;
      else
      {
        bHeight=true;
        g_wSpectrumHeight=l;
      }
    }
    else if (szArg.indexOf("stop",Qt::CaseInsensitive)==0)
    {
      int j=szArg.indexOf('=')+1;
      if (j<2) continue;
      long long l=atoll(szArg.toLatin1().constData()+j);
      if (l<1 || l>0x7FFFFFFFLL)
        bHelp=true;
      else
      {
        bStop=true;
        g_dwStopPacket=l;
      }
    }
    else if (szArg.indexOf("timer",Qt::CaseInsensitive)==0)
    {
      int j=szArg.indexOf('=')+1;
      if (j<2) continue;
      int l=atoi(szArg.toLatin1().constData()+j);
      if (l<1 || l>1000)
        bHelp=true;
      else
      {
        bTimer=true;
        g_wTimerInterval=l;
      }
    }
    else if (szArg.indexOf("v4",Qt::CaseInsensitive)==0)
      g_bV4=true;
    else
      bHelp=true;
  }
  if (bHelp)
    log(NULL,"command line arguments:\n" \
        "  --mcpd=<num>    initialize this number of MCPD-8 (1..64)\n" \
        "  --width=<num>   select number of channels for one MCPD-8 (1..64)\n" \
        "  --height=<num>  select number of positions for one channels (1..1024)\n" \
        "  --stop=<num>    stop after this number of packets\n" \
        "  --timer=<num>   timer interval (ms) for packet generator\n" \
        "  --v4            simulate \"round\" detector like V4/KWS/SANS@HZB\n");

  if (g_bV4)
  {
    if (!bMcpd)   g_wMCPD8 = 2;
    if (!bWidth)  g_wSpectrumWidth = 56;
    if (!bHeight) g_wSpectrumHeight = 960;
    if (!bTimer)  g_wTimerInterval = 10;
  }

  for (quint32 i=0; i<g_wMCPD8; ++i)
  {
    MCPD8* p=new MCPD8(this,(quint8)i,QHostAddress((quint32)(0x7F000002+i)),54321);
    if (p==NULL) continue;
    m_apMCPD8.append(p);
    connect(p,SIGNAL(CmdPacket(MDP_PACKET*,MCPD8*,QHostAddress&,quint16&)),this,SLOT(NewCmdPacket(MDP_PACKET*,MCPD8*,QHostAddress&,quint16&)),Qt::DirectConnection);
    log(p,"created %s",p->m_pSocket->localAddress().toString().toLatin1().constData());
  }

  QString szText;
  szText.sprintf("created %d MCPD-8 each with %d MPSD-8 (width=%u height=%u) and ",
                 m_apMCPD8.size(),(g_wSpectrumWidth+7)>>3,g_wSpectrumWidth,g_wSpectrumHeight);
  if (g_bV4)
    szText.append(QString().sprintf("round shape with %d different lengths",(int)((sizeof(v4Scale)/sizeof(v4Scale[0])+1)/2)));
  else
    szText+="rectangular shape";
  log(NULL,szText);

  ComputeSpectrum();
  m_aiPoints.clear();

  startTimer(g_wTimerInterval);
}

MainWindow::~MainWindow()
{
  for (int i=m_apMCPD8.size()-1; i>=0; --i)
  {
    MCPD8* p=m_apMCPD8.at(i);
    m_apMCPD8.remove(i);
    if (p==NULL) continue;
    disconnect(p,SIGNAL(CmdPacket(MDP_PACKET*,MCPD8*,QHostAddress&,quint16&)),this,SLOT(NewCmdPacket(MDP_PACKET*,MCPD8*,QHostAddress&,quint16&)));
    delete p;
  }
  delete ui;
}

void MainWindow::ComputeSpectrum(void)
{
  int iWidth=m_apMCPD8.size()*g_wSpectrumWidth;
  m_abySpectrum.clear();
  m_abySpectrum.resize(iWidth*g_wSpectrumHeight);
  if (iWidth<2)
  {
    for (int i=0; i<(int)g_wSpectrumHeight; ++i)
      m_abySpectrum[i]=1+(int)20.0*exp(-10.0*pow(2.0*(i+1)/g_wSpectrumHeight-1,2.0));
  }
  else
  {
    int xp,yp;
    QVector<quint8> abyTmp;

    abyTmp.clear();
    abyTmp.resize(iWidth*g_wSpectrumHeight);
    for (yp=0; yp<iWidth; ++yp)
    {
      for (xp=0; xp<(int)g_wSpectrumHeight; ++xp)
      {
        int l=yp*g_wSpectrumHeight+xp;
        double x = ((double)xp) / g_wSpectrumHeight;
        double y = ((double)yp) / iWidth;
        double value = 1.0;
        for (unsigned int k=0; k<sizeof(allPoints)/sizeof(allPoints[0]); ++k) {
          struct point *p = &allPoints[k];
          value += p->height *
        exp (p->y_exp *
             pow (2.0 * (y - p->y_min) / (p->y_max - p->y_min) - 1.0,
            2.0)) * exp (p->x_exp * pow (2.0 * (x -
                        p->x_min) /
                       (p->x_max - p->x_min) -
                       1.0, 2.0));
        }
        abyTmp[l]=(int)value;
      }
    }

    if (g_bV4)
    {
      for (yp=0; yp<iWidth; ++yp)
      {
        int y=yp*g_wSpectrumHeight;
        struct scale* pScale=&v4Scale[(yp*sizeof(v4Scale)/sizeof(v4Scale[0]))/iWidth];
        for (xp=0; xp<(int)g_wSpectrumHeight; ++xp)
        {
          int x=g_wSpectrumHeight*(pScale->src_start+xp*(pScale->src_end-pScale->src_start)/g_wSpectrumHeight);
          m_abySpectrum[y+xp]=quint8(abyTmp[y+x]*pScale->intensity);
        }
      }
    }
    else
      m_abySpectrum=abyTmp;
  }
}

quint64 MainWindow::GetClock(void)
{
  struct timespec tsNow;
  quint64 qwNow;
  clock_gettime(CLOCK_MONOTONIC,&tsNow);
  qwNow=tsNow.tv_sec;
  qwNow*=10000000;
  qwNow+=tsNow.tv_nsec/100;
  return qwNow;
}

void MainWindow::log(MCPD8* pMCPD8, const QString& szLine)
{
  QString sPrefix;
  if (pMCPD8!=NULL)
    sPrefix.sprintf("(%d) ",pMCPD8->m_byCpdId);
  ui->logText->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+sPrefix+szLine);
}

void MainWindow::log(MCPD8* pMCPD8, const QByteArray &abyLine)
{
  log(pMCPD8,QString(abyLine));
}

void MainWindow::log(MCPD8* pMCPD8, const char* szFormat, ...)
{
  QString szLine;
  va_list args;
  va_start(args,szFormat);
  szLine.vsprintf(szFormat,args);
  va_end(args);
  log(pMCPD8,szLine);
}

void MainWindow::NewCmdPacket(struct MDP_PACKET* pPacket, MCPD8* pMCPD8, QHostAddress &sender, quint16 &senderPort)
{
  quint16 wBufferLen=pPacket->bufferLength;
//log(pMCPD8,QByteArray((const char*)pPacket,2*pPacket->bufferLength));

  pPacket->bufferType|=0x8000; // CMDBUFTYPE
  pPacket->bufferLength=CMDHEADLEN;
  switch (pPacket->cmd)
  {
    case START:
    {
      m_qwMasterOffset=GetClock();
      pMCPD8->m_DataTarget=sender;
      pMCPD8->m_wDataPort=senderPort;
      StartStop(pMCPD8,true,"START via network");
      break;
    }
    case CONTINUE:
      StartStop(pMCPD8,true,"CONTINUE via network");
      break;
    case RESET:
      m_qwMasterOffset=GetClock();
      StartStop(pMCPD8,false,"RESET via network");
      break;
    case STOP:
      StartStop(pMCPD8,false,"STOP via network");
      break;
    case SETID:
      log(pMCPD8,"SETID %d",pPacket->data[0]&7);
      pMCPD8->m_byCpdId=pPacket->data[0]&7;
      break;
    case SETPROTOCOL:
      log(pMCPD8,"SETPROTOCOL ownip=%d.%d.%d.%d datareceiver=%d.%d.%d.%d cmdport=%u dataport=%u cmdreceiver=%d.%d.%d.%d",
          pPacket->data[0],pPacket->data[1],pPacket->data[2],pPacket->data[3],
          pPacket->data[4],pPacket->data[5],pPacket->data[6],pPacket->data[7],
          pPacket->data[8],pPacket->data[9],
          pPacket->data[10],pPacket->data[11],pPacket->data[12],pPacket->data[13]);
      break;
    case SETTIMING: // master, termination, external synchronisation
      log(pMCPD8,"SETTIMING");
      break;
    case SETRUNID:
      m_wRunId=pPacket->data[0];
      log(pMCPD8,"SETRUNID %u",m_wRunId);
      break;
    case SETCLOCK: // set master clock
    {
      quint64 qwNow=GetClock();
      quint64 qwMaster=(((quint64)pPacket->data[2])<<32) + (((quint64)pPacket->data[1])<<16) + pPacket->data[0];
      m_qwMasterOffset=qwNow-qwMaster;
      log(pMCPD8,"SETCLOCK %Lu (now=%Lu offset=%Lu)",qwMaster,qwNow,m_qwMasterOffset);
      break;
    }
//  case SETDAC:
//  case SENDSERIAL:
//  case READSERIAL:
//  case SCANPERI:
    case SETCELL:
      log(pMCPD8,"SETCELL source=%d trigger=%d compare=%d",pPacket->data[0],pPacket->data[1],pPacket->data[2]);
      // pPacket->data[0] = source;
      // pPacket->data[1] = trigger;
      // pPacket->data[2] = compare;
      break;
    case SETAUXTIMER:
      log(pMCPD8,"SETAUXTIMER timer=%d value=%d",pPacket->data[0],pPacket->data[1]);
      pPacket->bufferLength=CMDHEADLEN+2;
      // pPacket->data[0] = tim;
      // pPacket->data[1] = val;
      break;
    case SETPARAM:
      log(pMCPD8,"SETPARAM param=%d source=%d",pPacket->data[0],pPacket->data[1]);
      // pPacket->data[0] = param;
      // pPacket->data[1] = source;
      break;
//  case GETPARAM:
    case SETGAIN:
      log(pMCPD8,"SETGAIN addr=%d chan=%d gain=%d",pPacket->data[0],pPacket->data[1],pPacket->data[2]);
      pPacket->bufferLength=wBufferLen;
      // pPacket->data[0] = addr;
      // pPacket->data[1] = chan;
      // pPacket->data[2] = gainval;
      break;
    case SETTHRESH:
      log(pMCPD8,"SETTHRESH addr=%d threshold=%d",pPacket->data[0],pPacket->data[1]);
      pPacket->bufferLength=CMDHEADLEN+2;
      // pPacket->data[0] = addr;
      // pPacket->data[1] = thresh;
      break;
    case SETPULSER: // enable pulser
      log(pMCPD8,"SETPULSER addr=%d chan=%d pos=%d amp=%d onoff=%d",pPacket->data[0],pPacket->data[1],pPacket->data[2],pPacket->data[3],pPacket->data[4]);
      pPacket->bufferLength=CMDHEADLEN+5;
#warning "SETPULSER"
      // pPacket->data[0] = addr;
      // pPacket->data[1] = chan;
      // pPacket->data[2] = pos;
      // pPacket->data[3] = amp;
      // pPacket->data[4] = onoff;
      break;
    case SETMODE: // ==0 position mode, !=0 amplifier mode
      log(pMCPD8,"SETMODE addr=%d mode=%d",pPacket->data[0],pPacket->data[1]);
#warning "SETMODE"
      switch (pPacket->data[0])
      {
        case 0: // MPSD-8 #0
        case 1: // MPSD-8 #1
        case 2: // MPSD-8 #2
        case 3: // MPSD-8 #3
        case 4: // MPSD-8 #4
        case 5: // MPSD-8 #5
        case 6: // MPSD-8 #6
        case 7: // MPSD-8 #7
        default: // all
          break;
      }
      break;
    case GETCAPABILITIES: // MPSD-8 capabilities
#warning "GETCAPABILITIES: which types are supported"
      pPacket->bufferLength=CMDHEADLEN+8;
      pPacket->data[0]=1; // MPSD-8 capabilities (bit0=P, bit1=TP, bit2=TPA)
      pPacket->data[1]=(g_wSpectrumWidth> 8) ? 1 : 0;
      pPacket->data[2]=(g_wSpectrumWidth>16) ? 1 : 0;
      pPacket->data[3]=(g_wSpectrumWidth>24) ? 1 : 0;
      pPacket->data[4]=(g_wSpectrumWidth>32) ? 1 : 0;
      pPacket->data[5]=(g_wSpectrumWidth>40) ? 1 : 0;
      pPacket->data[6]=(g_wSpectrumWidth>48) ? 1 : 0;
      pPacket->data[7]=(g_wSpectrumWidth>56) ? 1 : 0;
      log(pMCPD8,"GETCAPABILITIES");
      break;
//  case SETCAPABILITIES:
//  case WRITEFPGA:
//  case READFPGA:
    case WRITEREGISTER:
      log(pMCPD8,"WRITEREGISTER addr=%d reg=%d val=%d",pPacket->data[0],pPacket->data[1],pPacket->data[2]);
      // pPacket->data[0] = addr;
      // pPacket->data[1] = reg;
      // pPacket->data[2] = val;
      break;
    case READREGISTER:
      pPacket->bufferLength=CMDHEADLEN+1;
      if (pPacket->data[1]==102)
        pPacket->data[0]=1; // MCPD-8 capabilities (bit0=P, bit1=TP, bit2=TPA)
      else
        pPacket->data[0]=0;
      log(pMCPD8,"READREGISTER reg=%d val=%d",pPacket->data[0],pPacket->data[1]);
      break;
//  case SETPOTI:
//  case GETPOTI:
    case READID: // read connected devices
      pPacket->bufferLength=CMDHEADLEN+8;
#warning "READID: which types are supported"
      pPacket->data[0]=TYPE_MPSD8;
      pPacket->data[1]=(g_wSpectrumWidth> 8) ? TYPE_MPSD8 : TYPE_NOMODULE;
      pPacket->data[2]=(g_wSpectrumWidth>16) ? TYPE_MPSD8 : TYPE_NOMODULE;
      pPacket->data[3]=(g_wSpectrumWidth>24) ? TYPE_MPSD8 : TYPE_NOMODULE;
      pPacket->data[4]=(g_wSpectrumWidth>32) ? TYPE_MPSD8 : TYPE_NOMODULE;
      pPacket->data[5]=(g_wSpectrumWidth>40) ? TYPE_MPSD8 : TYPE_NOMODULE;
      pPacket->data[6]=(g_wSpectrumWidth>48) ? TYPE_MPSD8 : TYPE_NOMODULE;
      pPacket->data[7]=(g_wSpectrumWidth>56) ? TYPE_MPSD8 : TYPE_NOMODULE;
      log(pMCPD8,"READID");
      break;
//  case DATAREQUEST:
//  case QUIET:
    case GETVER: // version 9.2
      pPacket->bufferLength=CMDHEADLEN+2;
      pPacket->data[0]=9;
      pPacket->data[1]=2;
      log(pMCPD8,"GETVER h=0x%04x l=0x%04x",pPacket->data[0],pPacket->data[1]);
      break;
    case READPERIREG: // read MPSD-8 module
      pPacket->bufferLength=CMDHEADLEN+3;
      switch (pPacket->data[1])
      {
        case 0: // MPSD-8 capabilities
          pPacket->data[2]=pPacket->data[0]<(g_wSpectrumWidth>>3) ? 1 : 0; // only P-mode supported
          break;
        case 1: // MPSD-8 mode (1=P, 2=TP, 4=TPA)
          pPacket->data[2]=pPacket->data[0]<(g_wSpectrumWidth>>3) ? 1 : 0;
          break;
        case 2: // MPSD-8 version (5.06)
          pPacket->data[2]=pPacket->data[0]<(g_wSpectrumWidth>>3) ? 0x0505 : 0x0000;
          break;
        default: // unknown register
          pPacket->data[2]=0x0000;
          break;
      }
      log(pMCPD8,"READPERIREG addr=%d reg=%d val=%d",pPacket->data[0],pPacket->data[1],pPacket->data[2]);
      break;
    case WRITEPERIREG:
      log(pMCPD8,"WRITEPERIREG addr=%d reg=%d val=%d",pPacket->data[0],pPacket->data[1],pPacket->data[2]);
      break;
    case SETMDLLTHRESHS:
    case SETMDLLSPECTRUM:
//  case SETMDLLMODE:
//  case SETMDLLHIST:
//  case SETMDLLSLSC:
    case SETMDLLPULSER:
    case SETMDLLDATASET:
    case SETMDLLACQSET:
    case SETMDLLEWINDOW:
      log(pMCPD8,"MDLL command 0x%04x",pPacket->cmd);
      pPacket->cmd|=0x8000; // command not supported
      // pPacket->data[0]=module
      // pPacket->data[1]=register
      // pPacket->data[2]=value
      break;
    default:
      log(pMCPD8,"unknown command 0x%04x",pPacket->cmd);
      pPacket->cmd|=0x8000; // command not supported
      break;
  }
  pMCPD8->Send(pPacket,sender,senderPort);
}

void MainWindow::on_action_Quit_triggered()
{
  close();
}

void MainWindow::on_action_StartStop_triggered()
{
  StartStop(NULL,!m_bDAQ,m_bDAQ?"STOP via menu":"START via menu");
}

void MainWindow::StartStop(MCPD8 *pMCPD8, bool bDAQ, const char *szReason)
{
  log(pMCPD8,"%s",szReason);
  m_bDAQ=bDAQ;
  ui->action_StartStop->setText(bDAQ?"&Stop":"&Start");
  m_dwPackets=0;
}

void MainWindow::timerEvent(QTimerEvent * event)
{
  QVector<struct DATA_PACKET> packets;
  (void)event;
  if (m_bDAQ)
  {
    quint64 qwHeaderTime=GetClock()-m_qwMasterOffset;
    unsigned int i,j,k;
    quint16 *p;

    if (m_aiPoints.size()<1)
    {
      m_aiPoints.clear();
      for (i=0; i<(unsigned)m_abySpectrum.size(); ++i)
        for (j=m_abySpectrum[i]; j>0; --j)
          m_aiPoints.append(i);
      ++m_qwLoopCount;
    }

#ifdef PRINTPACKET
    bool bPrintPacket=false;
    if (g_iPrintPacket>0) bPrintPacket=((--g_iPrintPacket)==0);
#endif
    packets.resize(m_apMCPD8.size());
    for (i=0; i<(unsigned int)m_apMCPD8.size(); ++i)
    {
      struct DATA_PACKET* pPacket=(struct DATA_PACKET*)(&packets[i]);
      memset(pPacket,0,sizeof(*pPacket));

      pPacket->bufferLength=0;
      //pPacket->bufferType=0x0002; // MDLL data buffer
      pPacket->bufferType  =0x0000; // data event buffer
      pPacket->headerLength=(sizeof(*pPacket)-sizeof(pPacket->data))/sizeof(quint16); // header length
      pPacket->bufferNumber=m_apMCPD8[i]->m_wBufferNo++;
      pPacket->runID       =m_wRunId;
      pPacket->deviceStatus=m_bDAQ ? 0x03 : 0x02; // bit 0: DAQ active, bit 1: SYNC ok
      pPacket->deviceId    =m_apMCPD8[i]->m_byCpdId;
      pPacket->time[0]     =qwHeaderTime&0xFFFF;
      pPacket->time[1]     =(qwHeaderTime>>16)&0xFFFF;
      pPacket->time[2]     =(qwHeaderTime>>32)&0xFFFF;

      pPacket->param[2][0] =m_qwLoopCount&0xFFFF;
      pPacket->param[2][1] =(m_qwLoopCount>>16)&0xFFFF;
      pPacket->param[2][2] =(m_qwLoopCount>>32)&0xFFFF;
      quint32 dwTime=QDateTime::currentDateTimeUtc().toTime_t();
      pPacket->param[3][0] =dwTime&0xFFFF;
      pPacket->param[3][1] =(dwTime>>16)&0xFFFF;
    }

    for (i=0; i<(unsigned int)packets.size(); ++i) for (j=0; j<10; ++j)
    {
      // Type(1)  ModID(3)  SlotID(5)  Amplitude(10)  Position(10)  Timestamp(19)
      // | TrigID(3)
      // | |   DataID(4)
      // | |   |              Data(21)
      // | |   |              |                   Timestamp(19)
      // | |   |              |                   |
      // | |  /\   +---------/ \--------++-------/ \---------+
      // |/ \/  \ /                     \/                   \a
      // YiiiSSSS dddddddd dddddddd dddddTTT TTTTTTTT TTTTTTTT
      // 22222222 22222222 11111111 11111111 00000000 00000000
      struct DATA_PACKET* pPacket=&packets[i];
      p=&pPacket->data[3*pPacket->bufferLength];
      k=qrand()&0x1F;
      switch (k) // trigger events
      {
        default:
          // additional chances for monitor counters
          if (k!=8 && k!=16 && k!=24 && // 3*MON1
              k!=9 && k!=17 &&          // 2*MON2
              k!=10)                    // 1*MON3
            break;
          k&=0x07;
          /*no break*/
        case 0: case 1: case 2: case 3: // monitor counter
        case 4: case 5: // rear TTL inputs 1,2
        case 6: case 7: // ADC inputs 1,2
        {
          pPacket->bufferLength++;
          p[2]=0x8000+(k<<8);
          k=qrand()&0x1FFFFF;
          p[2]|=k>>13;
          p[1]=(k&0x1FFF)<<3;
          p[0]=0x0000;
          break;
        }
      }
    }

    for (i=0; i<480; ++i)
    {
      if (m_aiPoints.size()<1) break;
      j=(j+qrand())%m_aiPoints.size();
      k=m_aiPoints.at(j);
      m_aiPoints.remove(j);
      if (k>=(unsigned)m_abySpectrum.size())
      {
#if 0
        log(NULL,"j=%u k=%u (max. %d)",j,k,m_abyStartSpectrum.size());
        Q_ASSERT(false);
#endif
        continue;
      }
      unsigned int y=k/g_wSpectrumHeight;
      quint16 mod=y/g_wSpectrumWidth;
      if (mod>=(unsigned int)m_apMCPD8.size())
      {
#if 0
        log(NULL,"j=%u k=%u mod=%u (max. %d)",j,k,mod,m_apMCPD8.size());
        Q_ASSERT(false);
#endif
        continue;
      }
      y%=g_wSpectrumWidth;
//      Q_ASSERT(mod<2);
//      Q_ASSERT(y<56);

      struct DATA_PACKET* pPacket=&packets[mod];
      p=&pPacket->data[3*(pPacket->bufferLength++)];

/*
      quint16 mod = pd.deviceId;
      quint8 id = (pd.data[counter + 2] >> 12) & 0x7;
      quint8 slotId = (pd.data[counter + 2] >> 7) & 0x1F;
      quint8 modChan = (id << 3) + slotId;
      quint16 chan = modChan + (mod << 6);
      quint16 amp = ((pd.data[counter+2] & 0x7F) << 3) + ((pd.data[counter+1] >> 13) & 0x7);
      quint16 pos = (pd.data[counter+1] >> 3) & 0x3FF;
*/

      // Type(1)  ModID(3)  SlotID(5)  Amplitude(10)  Position(10)  Timestamp(19)
      // | ModID(3)
      // | |   SlotID(5)
      // | |   |        Amplitude(10)
      // | |   |        |          Position(10)
      // | |   |        |          |               Timestamp(19)
      // | |   /\       |          |               |
      // | |  /  \  +--/ \--+  +--/ \--+  +-------/ \-------+
      // |/ \/    \/         \/         \/                   \a
      // YmmmSSSS Saaaaaaa aaaPPPPP PPPPPttt tttttttt tttttttt
      // 22222222 22222222 11111111 11111111 00000000 00000000
      p[2]|=0x0000;                   // Type
      p[2]|=((y>>3)&7)<<12;           // ModID
      p[2]|=(y&7)<<7;                 // SlotID
      p[2]|=0x0000;                   // Amplitude-HI

      p[1]|=0x0000;                   // Amplitude-LO
      p[1]|=(k%g_wSpectrumHeight)<<3; // Position
      p[1]|=0x0000;                   // Timestamp-HI

      p[0]=i;                    // Timestamp-LO

#ifdef PRINTPACKET
      if (bPrintPacket)
      {
        log(NULL,"i=%u j=%u k=%u mod=%u y=%u (%u/%u) - p[3]=%04x %04x %04x",
            i,j,k,mod,y,k%g_wSpectrumHeight,k/g_wSpectrumHeight,p[0],p[1],p[2]);
      }
#endif
#if 0
      //           TimeStamp-LO
      *p++=htole16(i&0xFFFF);
      //     Amplitude-LO   Position                     TimeStamp-HI
      *p++=htole16(0x0000 | ((k%g_wSpectrumHeight)<<3) | (i>>16));
      //           Type     ModID              SlotID       Amplitude-HI
      *p++=htole16(0x0000 | (((y>>3)&7)<<12) | ((y&7)<<4) | 0x0000);
#endif

      if (pPacket->bufferLength>=((sizeof(pPacket->data)-6-136)/6)) break;
    }

    for (i=0; i<(unsigned int)m_apMCPD8.size(); ++i)
    {
      struct DATA_PACKET* pPacket=(struct DATA_PACKET*)(&packets[i]);
      int iSize(m_aiPoints.size());
      pPacket->param[1][0] =iSize&0xFFFF;
      pPacket->param[1][1] =(iSize>>16)&0xFFFF;
    }

    for (i=0; i<(unsigned int)packets.size(); ++i)
    {
      struct DATA_PACKET* pPacket=(struct DATA_PACKET*)(&packets[i]);
      pPacket->bufferLength=(sizeof(*pPacket)-sizeof(pPacket->data))/sizeof(quint16)+(3*pPacket->bufferLength);
      m_apMCPD8[i]->Send(pPacket);
    }
#ifdef PRINTPACKET
    if (bPrintPacket)
    {
      for (i=0; i<(unsigned int)packets.size(); ++i)
      {
        struct DATA_PACKET* pPacket=(struct DATA_PACKET*)(&packets[i]);
        log(m_apMCPD8[i],HexDump(QByteArray((const char*)pPacket,pPacket->bufferLength*sizeof(quint16))));
        p=(quint16*)(&pPacket->data[0]);
        for (j=0; j<(unsigned int)((pPacket->bufferLength-pPacket->headerLength)/3); ++j, p+=3)
        {
          log(m_apMCPD8[i],"%u p[3]=%04x %04x %04x type=%u mod=%u slot=%u amp=%u pos=%u time=%u/%u",
              j,p[0],p[1],p[2],p[2]>>15,(p[2]>>12)&0x07,(p[2]>>4)&0x1F,((p[2]&0x7F)<<3)+(p[1]>>13),
              (p[1]>>3)&0x3FF,p[1]&0x07,p[0]);
        }
      }
    }
#endif

    if (g_dwStopPacket>0)
      if ((++m_dwPackets)>=g_dwStopPacket)
        StartStop(NULL,false,"STOP due packet counter");
  }
}

///////////////////////////////////////////////////////////////////////////////
// single instance of MCPD-8
///////////////////////////////////////////////////////////////////////////////

MCPD8::MCPD8(QObject *parent, quint8 id, const QHostAddress &address, quint16 port) :
  QObject(parent),
  m_pSocket(NULL),
  m_DataTarget(QHostAddress::Broadcast),
  m_wDataPort(54321),
  m_byCpdId(id),
  m_wBufferNo(0)
{
  m_pSocket=new QUdpSocket(this);
  if (m_pSocket->bind(address,port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint))
    connect(m_pSocket,SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::QueuedConnection);
  else
  {
    delete m_pSocket;
    m_pSocket=NULL;
  }
}

MCPD8::~MCPD8()
{
  if (m_pSocket!=NULL)
    disconnect(m_pSocket);
}

quint16 MCPD8::CalcCRC(const struct MDP_PACKET* pPacket)
{
  const quint16 *p=reinterpret_cast<const quint16 *>(pPacket);
  quint16 chksum=pPacket->headerChksum;
  for (quint32 i=0; i<pPacket->bufferLength; ++i)
    chksum^=p[i];
  return chksum;
}

void MCPD8::readyRead()
{
  while (m_pSocket->hasPendingDatagrams())
  {
    QHostAddress sender;
    quint16 senderPort;
    QByteArray datagram;
    struct MDP_PACKET* pPacket;
    QString szText;

    datagram.resize(m_pSocket->pendingDatagramSize());
    m_pSocket->readDatagram(datagram.data(),datagram.size(),&sender,&senderPort);

    pPacket=(struct MDP_PACKET*)datagram.data();
    if (datagram.size()>=(int)(sizeof(*pPacket)-sizeof(pPacket->data)) &&
        datagram.size()>=(int)pPacket->bufferLength)
    {
      if (pPacket->deviceId!=m_byCpdId)
        szText.sprintf("ignoring different MCPD id (mcpd=%d, other=%d)",m_byCpdId,pPacket->deviceId);
      else if (CalcCRC(pPacket)!=pPacket->headerChksum)
        szText.sprintf("invalid CRC 0x%04x 0x%04x",CalcCRC(pPacket),pPacket->headerChksum);
      else
      {
        emit CmdPacket(pPacket,this,sender,senderPort);
        return;
      }
    }
    else
      szText="unknown packet";

    MainWindow* pMainWindow=dynamic_cast<MainWindow*>(parent());
    if (pMainWindow!=NULL)
    {
      QString s(HexDump(datagram));
      pMainWindow->ui->logText->append(tr("%1: %2 data bytes from %3:%4 - %5").arg(szText).arg(datagram.size()).arg(sender.toString()).arg(senderPort).arg(s));
    }
  }
}

void MCPD8::Send(struct MDP_PACKET* pPacket, const QHostAddress &address, const quint16 wPort)
{
  qint64 qi;
  pPacket->headerChksum=0;
  pPacket->headerChksum=CalcCRC(pPacket);
  qi=pPacket->bufferLength;
  if (qi<100) qi=100;
  m_pSocket->writeDatagram((const char*)pPacket,qi,address,wPort);
}

void MCPD8::Send(struct DATA_PACKET* pPacket)
{
  m_pSocket->writeDatagram((const char*)pPacket,2*pPacket->bufferLength,m_DataTarget,m_wDataPort);
}

QByteArray HexDump(const QByteArray &data)
{
  QByteArray dst;
  const unsigned char* p=(const unsigned char*)data.constData();
  for (int i=0; i<data.size(); i+=16)
  {
    QString szLine;
    szLine.sprintf("%04x  ",i);
    for (int j=0; j<16; ++j)
    {
      QString szTmp;
      if (j==8) szLine+="- ";
      if ((i+j)<data.size())
        szTmp.sprintf("%02x ",p[i+j]);
      else
        szTmp="   ";
      szLine+=szTmp;
    }
    szLine+=" ";
    for (int j=0; j<16 && (i+j)<data.size(); ++j)
    {
      unsigned char c=p[i+j];
      if (c<32 || c>126) c='.';
      if (j==8) szLine+=" - ";
      szLine+=c;
    }
    dst+=szLine;
    dst+="\n";
  }
  return dst;
}
