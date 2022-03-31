#include "service.h"
#include "compass/plugin.h"

#include <QCompass>

Service::Service(const QString& rootDir)
: m_messenger()
, m_rootDir(rootDir)
{
  // register the generic compass
  m_compass = new BuiltInCompass();
  m_compass->setDataRate(2);
  m_sensor = new BuiltInSensorPlugin();
  m_sensor->registerSensors();
  m_SB = m_sensor->createBackend(m_compass);
}

Service::~Service()
{
  if (m_node)
  {
    m_node->disableRemoting(&m_messenger);
    delete m_tracker;
    delete m_node;
  }
  delete m_SB;
  delete m_sensor;
  delete m_compass;
  qInfo("%s", __FUNCTION__);
}

bool Service::run()
{
  m_tracker = new Tracker();
  m_tracker->init(m_rootDir);

  connect(m_compass, &BuiltInCompass::readingChanged, this, &Service::compassReading, Qt::QueuedConnection);
  m_SB->start();

  m_node = new QRemoteObjectHost(QUrl(QStringLiteral("local:replica")));
  return m_node->enableRemoting(&m_messenger);
}

void Service::compassSetActive(bool active)
{
  m_compass->setActive(active);
}

void Service::compassSetDataRate(int rate)
{
  m_compass->setDataRate(rate);
}

void Service::compassReading()
{
  qDebug("new data received: %d", m_compass->reading()->valueCount());
  for (int i = 0; i < m_compass->reading()->valueCount(); ++i)
  {
    m_messenger.pong(m_compass->reading()->value(i).toString());
    qDebug() << m_compass->reading()->value(i);
  }
}


/*
#include <QtGlobal>
#include <QGuiApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QTime>

#include <locale>

//#define Q_OS_ANDROID

#define APP_TR_NAME       "osmin"
#define ORG_NAME          "io.github.janbar"
#define APP_NAME          "osmin"
#define APP_DISPLAY_NAME  "Osmin"

#include "signalhandler.h"

#ifndef APP_VERSION
#define APP_VERSION "Undefined"
#endif

#define OSMIN_MODULE              "Osmin"
#define RES_GPX_DIR               "GPX"

#include "platformextras.h"
#include "converter.h"
#include "tracker.h"
#include "utils.h"

void setupApp(QGuiApplication& app);
void doExit(int code);

Tracker*          g_Tracker               = nullptr;

class aaa : public QObject {
public:
  aaa(QSensor * compass) :  _compass(compass) {}
public slots:
  void newData() {
    qDebug("new data received: %d", _compass->reading()->valueCount());
    for (int i = 0; i < _compass->reading()->valueCount(); ++i)
      qDebug() << _compass->reading()->value(i);
  }
private:
  QSensor * _compass = nullptr;
};

int main(int argc, char *argv[])
{
    int ret = 0;

    QGuiApplication::setApplicationName(APP_NAME);
    QGuiApplication::setApplicationDisplayName(APP_DISPLAY_NAME);
    QGuiApplication::setOrganizationName(ORG_NAME);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    setupApp(app);

    g_Tracker = new Tracker(&app);
    g_Tracker->init("/home/jlb/osmin/GPX");

    // register the generic compass
    QScopedPointer<BuiltInSensorPlugin> sensor(new BuiltInSensorPlugin());
    sensor->registerSensors();
    QScopedPointer<BuiltInCompass> compass(new BuiltInCompass());
    QSensorBackend * compassB = sensor->createBackend(compass.get());
    compassB->start();

    aaa * a = new aaa(compass.get());
    compass->connect(compass.get(), &BuiltInCompass::readingChanged, a, &aaa::newData, Qt::QueuedConnection);

    compassB->newReadingAvailable();

    QSettings settings;

    ret = app.exec();
    return ret;
}

void setupApp(QGuiApplication& app) {

    SignalHandler *sh = new SignalHandler(&app);
    sh->catchSignal(SIGHUP);
    sh->catchSignal(SIGALRM);
}
*/
