#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QUdpSocket>
#include <QVector>

namespace Ui {
  class MainWindow;
}

class MCPD8;
QByteArray HexDump(const QByteArray &data);

class MainWindow : public QMainWindow
{
  friend class MCPD8;
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void timerEvent(QTimerEvent * event);

private slots:
  void on_action_Quit_triggered();
  void on_action_StartStop_triggered();
  void NewCmdPacket(struct MDP_PACKET* pPacket, MCPD8* pMCPD8, QHostAddress &sender, quint16 &senderPort);

private:
  Ui::MainWindow* ui;

protected:
  QVector<MCPD8*> m_apMCPD8;
  bool            m_bDAQ;
  quint16         m_wRunId;
  quint64         m_qwMasterOffset;
  quint32         m_dwPackets;
  quint64         m_qwLoopCount;
  QVector<quint8> m_abySpectrum;
  QVector<int>    m_aiPoints;

  void ComputeSpectrum(void);
  static quint64 GetClock(void);
  void log(MCPD8* pMCPD8, const QString &szLine);
  void log(MCPD8* pMCPD8, const QByteArray &abyLine);
  void log(MCPD8* pMCPD8, const char* szFormat, ...);
  void StartStop(MCPD8 *pMCPD8, bool bDAQ, const char* szReason);
};

class MCPD8 : public QObject
{
  friend class MainWindow;
  Q_OBJECT

public:
  explicit MCPD8(QObject *parent=0, quint8 id=0, const QHostAddress &address=QHostAddress((quint32)0x7F000002), quint16 port=54321);
  virtual ~MCPD8();

  void Send(struct MDP_PACKET* pPacket, const QHostAddress &address, const quint16 wPort);
  void Send(struct DATA_PACKET* pPacket);

private slots:
  void readyRead();

signals:
  void CmdPacket(struct MDP_PACKET* pPacket, MCPD8* pMCPD8, QHostAddress &sender, quint16 &senderPort);

protected:
  QUdpSocket*     m_pSocket;
  QHostAddress    m_DataTarget;
  quint16         m_wDataPort;
  quint8          m_byCpdId;
  quint16         m_wBufferNo;

  static quint16 CalcCRC(const struct MDP_PACKET* pPacket);
};

#endif // MAINWINDOW_H
