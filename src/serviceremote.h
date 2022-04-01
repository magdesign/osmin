#ifndef SERVICEREMOTE_H
#define SERVICEREMOTE_H

#include <QObject>
#include <QThread>
#include "rep_servicemessenger_replica.h"

class ServiceRemote : public QObject
{
  Q_OBJECT
public:
  ServiceRemote();
  virtual ~ServiceRemote();

//public slots:

private slots:
  void run();
  void onStateChanged(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState);

private:
  QThread * m_t;
  QRemoteObjectNode * m_node = nullptr;
  QSharedPointer<ServiceMessengerReplica> m_messenger;
};

#endif // SERVICEREMOTE_H
