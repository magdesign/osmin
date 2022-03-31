#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QThread>

#include "servicemessenger.h"
#include "tracker.h"

class BuiltInCompass;
class BuiltInSensorPlugin;
class QSensorBackend;

class Service : public QObject
{
  Q_OBJECT
public:
  Service(const QString& rootDir);
  virtual ~Service();

public slots:
  bool run();
  void compassSetActive(bool active);
  void compassSetDataRate(int rate);

private:
  ServiceMessenger m_messenger;
  QString m_rootDir;
  BuiltInCompass * m_compass;
  BuiltInSensorPlugin * m_sensor;
  QSensorBackend * m_SB;

  QRemoteObjectHost * m_node = nullptr;
  Tracker * m_tracker = nullptr;

private slots:
  void compassReading();
};

#endif // SERVICE_H
