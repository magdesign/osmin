#include "servicetracker.h"
#include "serviceremote.h"

ServiceTracker::ServiceTracker(QObject *parent) : QObject(parent)
, m_vehicle(osmscout::Vehicle::vehicleCar)
, m_vehicleState(osmscout::PositionAgent::PositionState::NoGpsSignal)
, m_vehicleCoord(0, 0)
, m_vehicleBearing(osmscout::Bearing())
, m_elevation(0)
, m_currentSpeed(0)
, m_distance(0)
, m_duration(0)
, m_ascent(0)
, m_descent(0)
, m_maxSpeed(0)
, m_busy(false)
, m_recording("")
, m_isRecording(false)
{ }

osmscout::VehiclePosition *ServiceTracker::getTrackerPosition() const
{
  std::optional<osmscout::GeoCoord> nextPosition;
  //if (!m_nextRouteStep.getType().isEmpty())
  //  nextPosition = std::make_shared<osmscout::GeoCoord>(m_nextRouteStep.GetCoord());
  return new osmscout::VehiclePosition(m_vehicle, m_vehicleState, m_vehicleCoord, m_vehicleBearing, nextPosition);
}

void ServiceTracker::setRecording(const QString &filename)
{
  if (m_service)
    m_service->setRecording(filename);
}

void ServiceTracker::connectToService(ServiceRemote *service)
{
  if (m_service)
  {
    // callbacks
    disconnect(m_service, &ServiceRemote::trackerRecordingFailed, this, &ServiceTracker::recordingFailed);
    disconnect(m_service, &ServiceRemote::trackerResumeRecording, this, &ServiceTracker::resumeRecording);
    disconnect(m_service, &ServiceRemote::trackerIsRecordingChanged, this, &ServiceTracker::onTrackerIsRecordingChanged);
    disconnect(m_service, &ServiceRemote::trackerRecordingChanged, this, &ServiceTracker::onTrackerRecordingChanged);
    disconnect(m_service, &ServiceRemote::trackerProcessingChanged, this, &ServiceTracker::onTrackerProcessingChanged);
    disconnect(m_service, &ServiceRemote::trackerPositionChanged, this, &ServiceTracker::onTrackerPositionChanged);
    disconnect(m_service, &ServiceRemote::trackerPositionRecorded, this, &ServiceTracker::trackerPositionRecorded);
    disconnect(m_service, &ServiceRemote::trackerPositionMarked, this, &ServiceTracker::trackerPositionMarked);
    disconnect(m_service, &ServiceRemote::trackerDataChanged, this, &ServiceTracker::onTrackerDataChanged);
    // operations
    disconnect(this, &ServiceTracker::reset, m_service, &ServiceRemote::resetTrackingData);
    disconnect(this, &ServiceTracker::startRecording, m_service, &ServiceRemote::startRecording);
    disconnect(this, &ServiceTracker::stopRecording, m_service, &ServiceRemote::stopRecording);
    disconnect(this, &ServiceTracker::pinPosition, m_service, &ServiceRemote::pinfPosition);
    disconnect(this, &ServiceTracker::markPosition, m_service, &ServiceRemote::markPosition);
  }
  m_service = service;
  if (m_service)
  {
    // callbacks
    connect(m_service, &ServiceRemote::trackerRecordingFailed, this, &ServiceTracker::recordingFailed);
    connect(m_service, &ServiceRemote::trackerResumeRecording, this, &ServiceTracker::resumeRecording);
    connect(m_service, &ServiceRemote::trackerIsRecordingChanged, this, &ServiceTracker::onTrackerIsRecordingChanged);
    connect(m_service, &ServiceRemote::trackerRecordingChanged, this, &ServiceTracker::onTrackerRecordingChanged);
    connect(m_service, &ServiceRemote::trackerProcessingChanged, this, &ServiceTracker::onTrackerProcessingChanged);
    connect(m_service, &ServiceRemote::trackerPositionChanged, this, &ServiceTracker::onTrackerPositionChanged);
    connect(m_service, &ServiceRemote::trackerPositionRecorded, this, &ServiceTracker::trackerPositionRecorded);
    connect(m_service, &ServiceRemote::trackerPositionMarked, this, &ServiceTracker::trackerPositionMarked);
    connect(m_service, &ServiceRemote::trackerDataChanged, this, &ServiceTracker::onTrackerDataChanged);
    // operations
    connect(this, &ServiceTracker::reset, m_service, &ServiceRemote::resetTrackingData);
    connect(this, &ServiceTracker::startRecording, m_service, &ServiceRemote::startRecording);
    connect(this, &ServiceTracker::stopRecording, m_service, &ServiceRemote::stopRecording);
    connect(this, &ServiceTracker::pinPosition, m_service, &ServiceRemote::pinfPosition);
    connect(this, &ServiceTracker::markPosition, m_service, &ServiceRemote::markPosition);
  }
}

void ServiceTracker::onTrackerIsRecordingChanged(bool recording)
{
  m_isRecording = recording;
  emit trackerIsRecordingChanged();
}

void ServiceTracker::onTrackerRecordingChanged(const QString &filename)
{
  m_recording = filename;
  emit trackerRecordingChanged();
}

void ServiceTracker::onTrackerProcessingChanged(bool processing)
{
  m_busy = processing;
  emit trackerProcessingChanged();
}

void ServiceTracker::onTrackerPositionChanged(bool valid, double lat, double lon, double bearing)
{
  if (!valid)
    m_vehicleState = osmscout::PositionAgent::PositionState::NoGpsSignal;
  else
    m_vehicleState = osmscout::PositionAgent::PositionState::OffRoute;
  m_vehicleCoord.Set(lat, lon);
  m_vehicleBearing = osmscout::Bearing::Radians(bearing);
  emit trackerPositionChanged();
}

void ServiceTracker::onTrackerDataChanged(double elevation, double currentSpeed, double distance, double duration, double ascent, double descent, double maxSpeed)
{
  m_elevation = elevation;
  m_currentSpeed = currentSpeed;
  m_distance = distance;
  m_duration = duration;
  m_ascent = ascent;
  m_descent = descent;
  m_maxSpeed = maxSpeed;
  emit trackerDataChanged();
}
