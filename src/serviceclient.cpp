#include "serviceclient.h"
#include "rep_servicemessenger_replica.h"
#include <unistd.h>

ServiceClient::ServiceClient()
{
  m_t = new QThread();
  m_t->setObjectName("serviceclient");
  this->moveToThread(m_t);
  connect(m_t, &QThread::started, this, &ServiceClient::process);
  connect(m_t, &QThread::finished, m_t, &QThread::deleteLater);
  m_t->start();
}

ServiceClient::~ServiceClient()
{
  m_t->quit();
}

void ServiceClient::process()
{
  m_rnode = new QRemoteObjectNode();
  m_rnode->connectToNode(QUrl(QStringLiteral("local:replica")));
  m_rep = QSharedPointer<ServiceMessengerReplica>(m_rnode->acquire<ServiceMessengerReplica>());

  qWarning("################ Waiting service ...");

  bool res = m_rep->waitForSource();
  Q_ASSERT(res);

  qWarning("################ YOOOPPP started !!!!! : %s", (res ? "true" : "false"));

  connect(m_rep.data(), &ServiceMessengerReplica::pong, [](const QString &message){
      qWarning() << "Service sent: " << message;
  });

  m_rep->ping("Heeloooooo");
}
