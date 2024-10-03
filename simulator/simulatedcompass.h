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
#ifndef SIMULATEDCOMPASS_H
#define SIMULATEDCOMPASS_H

#include "globalazimuth.h"

#include <QObject>
#include <qsensorbackend.h>
#include <QTimer>

class SimulatedCompass : public QSensorBackend
{
  Q_OBJECT
public:
  static char const * const id;
  SimulatedCompass(GlobalAzimuth& azimuth, QSensor *sensor);
  ~SimulatedCompass();
  void start() Q_DECL_OVERRIDE;
  void stop() Q_DECL_OVERRIDE;

  void sensorError(int);

private slots:
  void onTimeout();

private:
  QTimer _updateTimer;

  GlobalAzimuth& _azimuth;
  QCompassReading _reading;
  static quint64 produceTimestamp();
};

#endif // SIMULATEDCOMPASS_H
