#include "serviceremote.h"
#include "rep_servicemessenger_replica.h"

#include <QTimer>
#include <unistd.h>
ServiceRemote::ServiceRemote(const QString& url)
: m_url(url)
, m_t(new QThread())
{
  m_t->setObjectName("serviceremote");
  this->moveToThread(m_t);
  connect(m_t, &QThread::finished, this, &ServiceRemote::onFinished);
  connect(m_t, &QThread::started, this, &ServiceRemote::run);
  m_t->start();
}

ServiceRemote::~ServiceRemote()
{
  if (!m_t->isFinished())
    terminate();
  m_t->deleteLater();
  qInfo("%s", __FUNCTION__);
}

void ServiceRemote::terminate()
{
  m_t->quit();
}

void ServiceRemote::setRecording(const QString &filename)
{
  m_messenger->tracker_setRecording(filename);
}

void ServiceRemote::resetTrackingData()
{
  m_messenger->tracker_resetData();
}

void ServiceRemote::startRecording()
{
  m_messenger->tracker_startRecording();
}

void ServiceRemote::stopRecording()
{
  m_messenger->tracker_stopRecording();
}

void ServiceRemote::pinfPosition()
{
  m_messenger->tracker_pinPosition();
}

void ServiceRemote::markPosition(const QString &symbol, const QString &name, const QString &description)
{
  m_messenger->tracker_markPosition(symbol, name, description);
}

void ServiceRemote::setCompassDataRate(int datarate)
{
  m_messenger->compass_setDataRate(datarate);
}

void ServiceRemote::setPositionUpdateInterval(int interval)
{
  m_messenger->position_setUpdateInterval(interval);
}

void ServiceRemote::setPreferedPositioningMethods(int methods)
{
  m_messenger->position_setPreferredPositioningMethods(methods);
}

void ServiceRemote::run()
{
  m_messenger.reset();
  if (m_node)
    delete m_node;
  m_node = new QRemoteObjectNode();
  m_node->connectToNode(QUrl(m_url));
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

  connect(m_messenger.data(), &ServiceMessengerReplica::pong, [](const QString& name){
      qWarning() << "Service reply: " << name;
  });

  connect(m_messenger.data(), &ServiceMessengerReplica::compass_readingChanged, this, &ServiceRemote::compassReadingChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::compass_activeChanged, this, &ServiceRemote::compassActiveChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::compass_dataRateChanged, this, &ServiceRemote::compassDataRateChanged);

  connect(m_messenger.data(), &ServiceMessengerReplica::position_positionUpdated, this, &ServiceRemote::positionPositionUpdated);
  connect(m_messenger.data(), &ServiceMessengerReplica::position_activeChanged, this, &ServiceRemote::positionActiveChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::position_updateIntervalChanged, this, &ServiceRemote::positionUpdateIntervalChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::position_preferredPositioningMethodsChanged, this, &ServiceRemote::positionPreferredPositioningMethodsChanged);

  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_recordingFailed, this, &ServiceRemote::trackerRecordingFailed);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_resumeRecording, this, &ServiceRemote::trackerResumeRecording);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_isRecordingChanged, this, &ServiceRemote::trackerIsRecordingChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_recordingChanged, this, &ServiceRemote::trackerRecordingChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_processingChanged, this, &ServiceRemote::trackerProcessingChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_positionRecorded, this, &ServiceRemote::trackerPositionRecorded);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_positionMarked, this, &ServiceRemote::trackerPositionMarked);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_positionChanged, this, &ServiceRemote::trackerPositionChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_dataChanged, this, &ServiceRemote::trackerDataChanged);

  connect(this, &ServiceRemote::ping, this, &ServiceRemote::onPing, Qt::QueuedConnection);
}

void ServiceRemote::onStateChanged(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState)
{
  (void)oldState;
  if (state != QRemoteObjectReplica::Valid)
    this->run();
}

void ServiceRemote::onPing(const QString &message)
{
  m_messenger->ping(message);
}

void ServiceRemote::onFinished()
{
  m_messenger.reset();
  if (m_node)
    delete m_node;
  m_node = nullptr;
}
