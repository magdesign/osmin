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
  connect(m_messenger.data(), &ServiceMessengerReplica::stateChanged, this, &ServiceRemote::onStateChanged);

  connect(m_messenger.data(), &ServiceMessengerReplica::pong, [](const QString &message){
      qWarning() << "Service sent: " << message;
  });
  m_messenger->ping("Hello...");
}

void ServiceRemote::onStateChanged(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState)
{
  if (state != QRemoteObjectReplica::Valid)
    this->run();
}
