#ifndef SERVICEFRONTEND_H
#define SERVICEFRONTEND_H

#include <osmscoutclientqt/VehiclePosition.h>
#include <osmscoutclientqt/RouteStep.h>
#include <osmscoutgpx/TrackPoint.h>
#include <osmscoutgpx/Waypoint.h>

#include <QObject>
#include <QThread>
#include "rep_servicemessenger_replica.h"

class ServiceFrontend;

typedef QSharedPointer<ServiceFrontend> ServiceFrontendPtr;

class ServiceFrontend : public QObject
{
  Q_OBJECT

public:
  ServiceFrontend(const QString& url);
  virtual ~ServiceFrontend();
  ServiceFrontend(const ServiceFrontend&) = delete;

  void terminate();

signals:
  // operations
  void ping(const QString& message);
  // callbacks
  void trackerRecordingFailed();
  void trackerResumeRecording();
  void trackerIsRecordingChanged(bool recording);
  void trackerRecordingChanged(const QString& filename);
  void trackerProcessingChanged(bool processing);
  void trackerPositionRecorded(double lat, double lon);
  void trackerPositionMarked(double lat, double lon, const QString& symbol, const QString& name);
  void trackerPositionChanged(bool valid, double lat, double lon, double bearing);
  void trackerDataChanged(
                double elevation,
                double currentSpeed,
                double distance,
                double duration,
                double ascent,
                double descent,
                double maxSpeed);

  void compassReadingChanged(float azimuth, float calibration);
  void compassDataRateChanged(int dataRate);
  void compassActiveChanged(bool active);

  void positionPositionUpdated(bool valid, double lat, double lon, bool haccvalid, float hacc, double alt);
  void positionActiveChanged(bool active);
  void positionUpdateIntervalChanged(int interval);
  void positionPreferredPositioningMethodsChanged(int methods);

  void serviceDisconnected();
  void serviceConnected();

public slots:
  void setRecording(const QString& filename);
  void resetTrackingData();
  void startRecording();
  void stopRecording();
  void pinfPosition();
  void markPosition(const QString& symbol, const QString& name, const QString& description);
  void setCompassDataRate(int datarate);
  void setPositionUpdateInterval(int interval);
  void setPreferedPositioningMethods(int methods);

private slots:
  void run();
  void onStateChanged(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState);
  void onPing(const QString& message);
  void onFinished();

private:
  QString m_url;
  QThread * m_t;
  QRemoteObjectNode * m_node = nullptr;
  QSharedPointer<ServiceMessengerReplica> m_messenger;
};

#endif // SERVICEFRONTEND_H
