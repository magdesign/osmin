#include "service.h"
#include <unistd.h>

Service::Service()
{
  m_t = new QThread();
  m_t->setObjectName("service");
  this->moveToThread(m_t);
  connect(m_t, &QThread::started, this, &Service::process);
  connect(m_t, &QThread::finished, m_t, &QThread::deleteLater);
  m_t->start();
}

Service::~Service()
{
  m_t->quit();
}

void Service::process()
{
  m_node = new QRemoteObjectHost(QUrl(QStringLiteral("local:replica")));
  m_sm = new ServiceMessenger();
  m_node->enableRemoting(m_sm);
  qWarning("SERVICE STARTED");
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
#include <QCompass>

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
#include "compass/plugin.h"

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
