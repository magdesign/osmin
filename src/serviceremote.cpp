#include "serviceremote.h"
#include "rep_servicemessenger_replica.h"

#include <QTimer>

ServiceRemote::ServiceRemote()
: m_t(new QThread())
{
  m_t->setObjectName("serviceremote");
  this->moveToThread(m_t);
  connect(m_t, &QThread::finished, m_t, &QThread::deleteLater);
  connect(m_t, &QThread::started, this, &ServiceRemote::run);
  m_t->start();
}

ServiceRemote::~ServiceRemote()
{
  m_messenger.reset();
  if (m_node)
    delete m_node;
  m_t->quit();
}

void ServiceRemote::run()
{
  m_messenger.reset();
  if (m_node)
    delete m_node;
  m_node = new QRemoteObjectNode();
  m_node->connectToNode(QUrl(QStringLiteral("local:replica")));
  m_messenger.reset(m_node->acquire<ServiceMessengerReplica>());
  bool res = false;
  qDebug("Trying to connect to service ...");
  while (!res)
  {
    res = m_messenger->waitForSource();
    if (!res)
      qDebug("Connect to service source failed after timeout");
  }
  qDebug("Connect to service succeeded");
  // setup watch dog
  connect(m_messenger.data(), &ServiceMessengerReplica::stateChanged, this, &ServiceRemote::onStateChanged, Qt::QueuedConnection);

  connect(m_messenger.data(), &ServiceMessengerReplica::compass_readingChanged, [](float a, float c){
      qWarning() << "COMPASS: " << a << c;
  });
  connect(m_messenger.data(), &ServiceMessengerReplica::pong, [](const QString& name){
      qWarning() << "Service sent: " << name;
  });
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_DataChanged, [](double elevation,
          double currentSpeed,
          double distance,
          double duration,
          double ascent,
          double descent,
          double maxSpeed){
      qWarning() << "DATA: " << elevation << currentSpeed << distance << duration << ascent << descent << maxSpeed;
  });
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_PositionRecorded, [](double lat, double lon){
      qWarning() << "RECORDED: " << lat << lon;
  });
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_PositionMarked, [](double lat, double lon, const QString& symbol, const QString& name){
      qWarning() << "MARKED: " << lat << lon << symbol << name;
  });
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_RecordingChanged, [](const QString& filename){
      qWarning() << "RECORDING: " << filename;
  });
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_IsRecordingChanged, [](bool recording){
      qWarning() << "IS RECORDING: " << recording;
  });
  connect(m_messenger.data(), &ServiceMessengerReplica::position_positionUpdated, [](
          bool positionValid,
          double latitude,
          double longitude,
          bool horizontalAccuracyValid,
          float horizontalAccuracy,
          double altitude){
      qWarning() << "POSITION: " << positionValid << latitude << longitude << altitude;
  });

  m_messenger->ping("Hello...");
  //m_messenger->tracker_StartRecording();
}

void ServiceRemote::onStateChanged(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState)
{
  if (state != QRemoteObjectReplica::Valid)
    this->run();
}
