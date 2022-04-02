#include "servicepositionsource.h"
#include "serviceremote.h"

void ServicePositionSource::setActive(bool active)
{
  if (m_service)
  {
    if (!m_active && active)
    {
      connect(m_service, &ServiceRemote::positionPositionUpdated, this, &ServicePositionSource::onPositionUpdated);
      connect(m_service, &ServiceRemote::positionUpdateIntervalChanged, this, &ServicePositionSource::onUpdateIntervalChanged);
      connect(m_service, &ServiceRemote::positionPreferredPositioningMethodsChanged, this, &ServicePositionSource::onPreferredPositioningMethodsChanged);
      m_active = true;
      emit activeChanged();
    }
    else if (m_active && !active)
    {
      disconnect(m_service, &ServiceRemote::positionPositionUpdated, this, &ServicePositionSource::onPositionUpdated);
      disconnect(m_service, &ServiceRemote::positionUpdateIntervalChanged, this, &ServicePositionSource::onUpdateIntervalChanged);
      disconnect(m_service, &ServiceRemote::positionPreferredPositioningMethodsChanged, this, &ServicePositionSource::onPreferredPositioningMethodsChanged);
      m_active = false;
      emit activeChanged();
    }
  }
}

void ServicePositionSource::connectToService(ServiceRemote *service)
{
  if (m_service)
  {
    setActive(false);
    //disconnect(this, &ServicePositionSource::setUpdateInterval, m_service, &ServiceRemote::setPositionUpdateInterval);
    disconnect(this, &ServicePositionSource::setPositioningMethods, m_service, &ServiceRemote::setPreferedPositioningMethods);
  }
  m_service = service;
  if (m_service)
  {
    //connect(this, &ServicePositionSource::setUpdateInterval, m_service, &ServiceRemote::setPositionUpdateInterval);
    connect(this, &ServicePositionSource::setPositioningMethods, m_service, &ServiceRemote::setPreferedPositioningMethods);
    setActive(true);
  }
}

void ServicePositionSource::onPositionUpdated(bool valid, double lat, double lon, bool haccvalid, float hacc, double alt)
{
  m_position.set(valid, lat, lon, haccvalid, hacc, alt);
  emit positionChanged();
}

void ServicePositionSource::onUpdateIntervalChanged(int interval)
{
  m_updateInterval = interval;
  emit updateIntervalChanged();

}

void ServicePositionSource::onPreferredPositioningMethodsChanged(int methods)
{
  m_positioningMethods = methods;
  emit preferredPositioningMethodsChanged();
}

