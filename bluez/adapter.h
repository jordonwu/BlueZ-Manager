/*
 * Qt BlueZ Manager
 *
 * Copyright (C) 2010 Jose Antonio Santos-Cadenas <santoscadenas at gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef ADAPTER_H
#define ADAPTER_H

#include <QObject>

#include <QtDBus/QtDBus>

class Adapter : public QObject
{
	Q_OBJECT

public:
	Adapter(QString path);
	Adapter(Adapter &adapter);
	~Adapter();
	QVariantMap getProperties();
	void setProperty(QString key, QVariant value);

	QString getPath();

signals:
	void deviceAdded(QString devPath);
	void deviceRemoved(QString devPath);
	void propertyChanged(QString key, QVariant value);

private slots:
	void slotDeviceRemoved(QDBusObjectPath path);
	void slotDeviceAdded(QDBusObjectPath path);
	void slotPropertyChanged(QString name, QDBusVariant value);

private:
	void setSignals();

	QDBusInterface adapter;
};

#endif // ADAPTER_H
