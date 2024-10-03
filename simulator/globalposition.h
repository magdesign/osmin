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
#ifndef GLOBALPOSITION_H
#define GLOBALPOSITION_H

#include <mutex>
#include <QObject>
#include <QtPositioning/QGeoPositionInfoSource>

class GlobalPosition : public QObject
{
  Q_OBJECT

public:
  GlobalPosition() { }
  ~GlobalPosition() = default;

  void resetData(double lat, double lon, double alt);
  QGeoPositionInfo data() const;

signals:
  void dataUpdated();

private:
  mutable std::mutex _mutex;
  QGeoPositionInfo _info;
};

#endif // GLOBALPOSITION_H
