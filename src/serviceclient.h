#ifndef SERVICECLIENT_H
#define SERVICECLIENT_H

#include <QObject>
#include <QThread>
#include "rep_servicemessenger_replica.h"

class ServiceClient : public QObject
{
  Q_OBJECT
public:
  ServiceClient();
  virtual ~ServiceClient();

  void process();

private:
  QRemoteObjectNode * m_rnode;
  QSharedPointer<ServiceMessengerReplica> m_rep;
  QThread * m_t;
};

#endif // SERVICECLIENT_H
