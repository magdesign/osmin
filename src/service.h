#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QThread>
#include <QGeoPositionInfo>

#include "rep_servicemessenger_source.h"
#include "tracker.h"

class BuiltInCompass;
class BuiltInSensorPlugin;
class QSensorBackend;
class QGeoPositionInfoSource;

class Service : public ServiceMessengerSource
{
  Q_OBJECT
public:
  Service(const QString& rootDir);
  virtual ~Service();

signals:
  void compass_azimuthChanged(double azimuth);

public slots:
  bool run();

  void ping(const QString &message) override;

  void compass_setActive(bool active) override;
  void compass_setDataRate(int dataRate) override;

  void position_setUpdateInterval(int interval) override;
  void position_setPreferredPositioningMethods(int methods) override;

  void tracker_SetRecording(const QString& filename) override;
  void tracker_StartRecording() override;
  void tracker_ResumeRecording(const QString& filename) override;
  void tracker_StopRecording() override;
  void tracker_PinPosition() override;
  void tracker_MarkPosition(const QString& symbol, const QString& name, const QString& description) override;
  void tracker_ResetData() override;

private slots:
  void onCompassReadingChanged();
  void onCompassactiveChanged();
  void onCompassdataRateChanged();

  void onPositionPositionUpdated(QGeoPositionInfo info);
  void onPositionUpdateIntervalChanged();
  void onPositionSupportedPositioningMethodsChanged();

  void onTrackerRecordingChanged();
  void onTrackerProcessingChanged();
  void onTrackerPositionRecorded(double lat, double lon);
  void onTrackerPositionMarked(double lat, double lon, const QString& symbol, const QString& name);
  void onTrackerRecordingFailed();
  void onTrackerDataChanged();

private:
  QString m_rootDir;
  BuiltInCompass * m_compass;
  BuiltInSensorPlugin * m_sensor;
  QSensorBackend * m_SB;

  QGeoPositionInfoSource * m_position;

  QRemoteObjectHost * m_node = nullptr;
  Tracker * m_tracker = nullptr;
};

#endif // SERVICE_H
