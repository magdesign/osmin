#ifndef SERVICECOMPASS_H
#define SERVICECOMPASS_H

#include <QObject>

class ServiceRemote;

class ServiceCompassReading : public QObject
{
  Q_OBJECT
  Q_PROPERTY(float azimuth READ getAzimuth CONSTANT)
  Q_PROPERTY(float calibrationLevel READ getCalibrationLevel CONSTANT)

public:
  explicit ServiceCompassReading(QObject * parent = nullptr) : QObject(parent) { }

  float getAzimuth() { return m_azimuth; }
  float getCalibrationLevel() { return m_calibrationLevel; }
  void set(float azimuth, float calibration)
  {
    m_azimuth = azimuth;
    m_calibrationLevel = calibration;
  }
private:
  float m_azimuth = 0.0f;
  float m_calibrationLevel = 0.0f;
};

class ServiceCompass : public QObject
{
  Q_OBJECT
  Q_PROPERTY(ServiceCompassReading* reading READ getReading NOTIFY readingChanged)
  Q_PROPERTY(bool active READ getActive WRITE setActive NOTIFY activeChanged)
  Q_PROPERTY(int dataRate READ getDataRate WRITE setDataRate NOTIFY dataRateChanged)

public:
  explicit ServiceCompass(QObject * parent = nullptr) : QObject(parent), m_reading(this) { }

  ServiceCompassReading* getReading() { return &m_reading; }
  bool getActive() { return m_active; }
  int getDataRate() { return m_dataRate; }
  void setActive(bool active);
  void setDataRate(int datarate) { (void)datarate; /*disabled*/ }

  // for testing
  void set(float azimuth, float calibration)
  {
    onReadingChanged(azimuth, calibration);
  }

  Q_INVOKABLE void connectToService(ServiceRemote * service);

signals:
  // operations
  // callbacks
  void readingChanged();
  void activeChanged();
  void dataRateChanged();

private slots:
  void onReadingChanged(float azimuth, float calibration);
  void onDataRateChanged(int datarate);

private:
  ServiceRemote * m_service = nullptr;
  ServiceCompassReading m_reading;
  bool m_active = false;
  int m_dataRate = 0;
};

#endif // SERVICECOMPASS_H
