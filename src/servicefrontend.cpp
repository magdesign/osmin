#include "servicefrontend.h"
#include "rep_servicemessenger_replica.h"

#include <QTimer>
#include <unistd.h>
ServiceFrontend::ServiceFrontend(const QString& url)
: m_url(url)
, m_t(new QThread())
{
  m_t->setObjectName("serviceremote");
  this->moveToThread(m_t);
  connect(m_t, &QThread::finished, this, &ServiceFrontend::onFinished);
  connect(m_t, &QThread::started, this, &ServiceFrontend::run);
  m_t->start();
}

ServiceFrontend::~ServiceFrontend()
{
  if (!m_t->isFinished())
    terminate();
  m_t->deleteLater();
  qInfo("%s", __FUNCTION__);
}

void ServiceFrontend::terminate()
{
  m_t->quit();
}

void ServiceFrontend::setRecording(const QString &filename)
{
  m_messenger->tracker_setRecording(filename);
}

void ServiceFrontend::resetTrackingData()
{
  m_messenger->tracker_resetData();
}

void ServiceFrontend::startRecording()
{
  m_messenger->tracker_startRecording();
}

void ServiceFrontend::stopRecording()
{
  m_messenger->tracker_stopRecording();
}

void ServiceFrontend::pinfPosition()
{
  m_messenger->tracker_pinPosition();
}

void ServiceFrontend::markPosition(const QString &symbol, const QString &name, const QString &description)
{
  m_messenger->tracker_markPosition(symbol, name, description);
}

void ServiceFrontend::setCompassDataRate(int datarate)
{
  m_messenger->compass_setDataRate(datarate);
}

void ServiceFrontend::setPositionUpdateInterval(int interval)
{
  m_messenger->position_setUpdateInterval(interval);
}

void ServiceFrontend::setPreferedPositioningMethods(int methods)
{
  m_messenger->position_setPreferredPositioningMethods(methods);
}

void ServiceFrontend::run()
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
  connect(m_messenger.data(), &ServiceMessengerReplica::stateChanged, this, &ServiceFrontend::onStateChanged, Qt::QueuedConnection);

  connect(m_messenger.data(), &ServiceMessengerReplica::pong, [](const QString& name){
      qWarning() << "Service reply: " << name;
  });

  connect(m_messenger.data(), &ServiceMessengerReplica::compass_readingChanged, this, &ServiceFrontend::compassReadingChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::compass_activeChanged, this, &ServiceFrontend::compassActiveChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::compass_dataRateChanged, this, &ServiceFrontend::compassDataRateChanged);

  connect(m_messenger.data(), &ServiceMessengerReplica::position_positionUpdated, this, &ServiceFrontend::positionPositionUpdated);
  connect(m_messenger.data(), &ServiceMessengerReplica::position_activeChanged, this, &ServiceFrontend::positionActiveChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::position_updateIntervalChanged, this, &ServiceFrontend::positionUpdateIntervalChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::position_preferredPositioningMethodsChanged, this, &ServiceFrontend::positionPreferredPositioningMethodsChanged);

  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_recordingFailed, this, &ServiceFrontend::trackerRecordingFailed);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_resumeRecording, this, &ServiceFrontend::trackerResumeRecording);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_isRecordingChanged, this, &ServiceFrontend::trackerIsRecordingChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_recordingChanged, this, &ServiceFrontend::trackerRecordingChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_processingChanged, this, &ServiceFrontend::trackerProcessingChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_positionRecorded, this, &ServiceFrontend::trackerPositionRecorded);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_positionMarked, this, &ServiceFrontend::trackerPositionMarked);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_positionChanged, this, &ServiceFrontend::trackerPositionChanged);
  connect(m_messenger.data(), &ServiceMessengerReplica::tracker_dataChanged, this, &ServiceFrontend::trackerDataChanged);

  connect(this, &ServiceFrontend::ping, this, &ServiceFrontend::onPing, Qt::QueuedConnection);

  emit serviceConnected();
}

void ServiceFrontend::onStateChanged(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState)
{
  (void)oldState;
  if (state != QRemoteObjectReplica::Valid)
  {
    emit serviceDisconnected();
    this->run();
  }
}

void ServiceFrontend::onPing(const QString &message)
{
  m_messenger->ping(message);
}

void ServiceFrontend::onFinished()
{
  m_messenger.reset();
  if (m_node)
    delete m_node;
  m_node = nullptr;
}
