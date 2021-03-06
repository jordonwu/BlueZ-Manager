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

#ifndef MANAGER_AGENT_H
#define MANAGER_AGENT_H

#include <QObject>

#include <QDBusObjectPath>
#include <QDBusMessage>

class ManagerAgent : public QObject
{
	Q_OBJECT

public:
	ManagerAgent(const QString &path, QObject *parent = 0);
	~ManagerAgent();

	QString adapterPath();
	void setPinCode(QString &pin);

public slots:
	/* D-Bus Agent slots */
	void Authorize(const QDBusObjectPath &device, const QString &uuid);
	void Cancel();
	void ConfirmModeChange(const QString &mode);
	void DisplayPasskey(const QDBusObjectPath &device, uint passkey);
	void Release();
	void RequestConfirmation(const QDBusObjectPath &device, uint passkey);
	uint RequestPasskey(const QDBusObjectPath &device);
	QString RequestPinCode(const QDBusObjectPath &device,
						const QDBusMessage &message);

private:
	QString path;
	QString *pinCode;

};

#endif // MANAGER_AGENT_H
