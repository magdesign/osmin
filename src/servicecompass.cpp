#include "servicecompass.h"
#include "serviceremote.h"

void ServiceCompass::setActive(bool active)
{
  if (m_service)
  {
    if (!m_active && active)
    {
      connect(m_service, &ServiceRemote::compassReadingChanged, this, &ServiceCompass::onReadingChanged);
      connect(m_service, &ServiceRemote::compassDataRateChanged, this, &ServiceCompass::onDataRateChanged);
      m_active = true;
      emit activeChanged();
    }
    else if (m_active && !active)
    {
      disconnect(m_service, &ServiceRemote::compassReadingChanged, this, &ServiceCompass::onReadingChanged);
      disconnect(m_service, &ServiceRemote::compassDataRateChanged, this, &ServiceCompass::onDataRateChanged);
      m_active = false;
      emit activeChanged();
    }
  }
}


void ServiceCompass::connectToService(ServiceRemote *service)
{
  if (m_service)
  {
    setActive(false);
    //disconnect(this, &ServiceCompass::setDataRate, m_service, &ServiceRemote::setCompassDataRate);
  }
  m_service = service;
  if (m_service)
  {
    //connect(this, &ServiceCompass::setDataRate, m_service, &ServiceRemote::setCompassDataRate);
    setActive(true);
  }
}

void ServiceCompass::onReadingChanged(float azimuth, float calibration)
{
  m_reading.set(azimuth, calibration);
  emit readingChanged();
}

void ServiceCompass::onDataRateChanged(int datarate)
{
  m_dataRate = datarate;
  emit dataRateChanged();
}
