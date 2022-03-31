#include "rep_servicemessenger_source.h"
#include <QDebug>

class ServiceMessenger : public ServiceMessengerSource {
public slots:
  void ping(const QString &name) override {
    qWarning("##### SERVICE RECEIVDED : %s", name.toUtf8().constData());
    emit pong("Hello >>>> " + name);
  }
};
