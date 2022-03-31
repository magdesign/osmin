#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QThread>
#include "servicemessenger.h"

class Service : public QObject
{
  Q_OBJECT
public:
  Service();
  virtual ~Service();

  void process();

private:
  QRemoteObjectHost * m_node;
  ServiceMessenger * m_sm;
  QThread * m_t;
};

#endif // SERVICE_H
