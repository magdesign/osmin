/*
 * Copyright (C) 2024
 *      Jean-Luc Barriere <jlbarriere68@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "simulatorbreaker.h"
#include "simulatedsensorplugin.h"
#include "simulatedpositionsource.h"
#include "commandline.h"
#include "gpxrunner.h"
#include "scriptrunner.h"

#include <converter.h>

#include <QObject>

class Simulator : public QObject, public SimulatorBreaker
{
  Q_OBJECT

public:
  Simulator();
  ~Simulator();

  void enableCLI(CommandLine * cmd);
  void disableCLI();

  QSensor * compassSensor() { return _compassSource; }
  QGeoPositionInfoSource * positionSource() { return _positionSource; }

  bool onKeyBreak() override;

  bool scriptRunning() { return !_scripts.empty(); }
  bool scriptAborted() { return !_aborted.empty(); }

signals:
  void prompted();

public slots:
  void onCommand(QString line);
  void onQuit();
  void onListGPXRequested();
  void onStatusRequested();
  void onPointChanged(int pts);
  void onGPXFinished();
  void onScriptFinished();

private:
  GlobalAzimuth _azimuth;
  GlobalPosition _position;

  SimulatedSensorPlugin * _compassPlugin = nullptr;
  SimulatedSensor * _compassSource = nullptr;
  SimulatedPositionSource * _positionSource = nullptr;

  GPXRunner * _gpxrunner;
  QList<ScriptRunner*> _scripts;
  QList<ScriptRunner*> _aborted;
  CommandLine * _cmd = nullptr;
  Converter _converter;
  bool _prompt = true;

  void prompt();
  static qreal normalizeAzimuth(qreal azimuth);
  QStringList tokenize(const char * buf);
  bool loopDetected(const ScriptRunner& script) const;
};

#endif // SIMULATOR_H
