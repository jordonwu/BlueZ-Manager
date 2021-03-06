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

#include <QMenu>

#include "deviceview.h"
#include "ui_deviceview.h"
#include "changenamedialog.h"

#include <knotification.h>
#include <klocalizedstring.h>

DeviceView::DeviceView(QString path, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DeviceView),
	device("org.bluez", path, QDBusConnection::systemBus()),
	properties()
{
	ui->setupUi(this);

	QDBusPendingReply<QVariantMap> dprops = device.GetProperties();
	dprops.waitForFinished();

	if (!dprops.isValid()) {
		qCritical() << "Received unvalid reply";
		return;
	}

	properties = dprops.value();

	ui->device->setTitle(properties["Alias"].toString());
	ui->realName->setText(properties["Name"].toString());
	ui->address->setText(properties["Address"].toString());
	setConnection(properties["Connected"].toBool());
	setTrusted(properties["Trusted"].toBool());

	connect(&device, SIGNAL(PropertyChanged(QString,QDBusVariant)), this,
				SLOT(propertyChanged(QString, QDBusVariant)));

	connect(ui->connected, SIGNAL(clicked()), this,
						SLOT(connectedClicked()));
	connect(ui->trusted, SIGNAL(clicked()), this, SLOT(trustedClicked()));

	connect(ui->device, SIGNAL(customContextMenuRequested(const QPoint &)),
				this, SLOT(contextMenu(const QPoint &)));
}

DeviceView::~DeviceView()
{
	delete ui;
}

void DeviceView::setConnection(bool connected)
{
	ui->connected->setChecked(connected);
	ui->connected->setEnabled(connected);
	if (connected)
		ui->connected->setToolTip(tr("Connected"));
	else
		ui->connected->setToolTip(tr("Disconnected"));
}

void DeviceView::setTrusted(bool trusted)
{
	ui->trusted->setChecked(trusted);
	if (trusted)
		ui->trusted->setToolTip(tr("Trusted device"));
	else
		ui->trusted->setToolTip(tr("Untrusted device"));
}

QString DeviceView::devicePath()
{
	return device.path();
}

void DeviceView::propertyChanged(const QString &name, const QDBusVariant &value)
{
	if (name == "Connected") {
		setConnection(value.variant().toBool());
	} else if (name == "Alias") {
		ui->device->setTitle(value.variant().toString());
	} else if (name == "Name") {
		ui->realName->setText(value.variant().toString());
	} else if (name == "Trusted") {
		setTrusted(value.variant().toBool());
	}

	properties[name] = value.variant();
}

void DeviceView::connectedClicked()
{
	if (!ui->connected->isEnabled())
		return;

	// Is only enabled if is already connected.
	ui->connected->setCheckState(Qt::PartiallyChecked);
	ui->connected->setToolTip(tr("Disconnecting"));
	ui->connected->setEnabled(FALSE);

	device.Disconnect();
}

void DeviceView::trustedClicked()
{
	QDBusPendingReply<> reply = device.SetProperty("Trusted",
					QDBusVariant(ui->trusted->isChecked()));
	reply.waitForFinished();

	if (reply.isError()) {
		ui->trusted->setChecked(!ui->trusted->isChecked());
		qWarning() << "Error setting property";
	}
}

void DeviceView::newNameSet(const QString &name)
{
	device.SetProperty("Alias", QDBusVariant(name));
}

/* Context menu */
void DeviceView::contextMenu(const QPoint &pos)
{
	QMenu *menu = new QMenu(this);
	menu->addAction(tr("Delete pairing"), this, SLOT(deletePairing()));
	menu->addAction(tr("Change name"), this, SLOT(changeName()));
	menu->exec(ui->device->mapToGlobal(pos));
}

void DeviceView::deletePairing()
{
	emit deletePairing(device.path());
}

void DeviceView::changeName()
{
	ChangeNameDialog *cnd = new ChangeNameDialog(
					properties["Alias"].toString(), this);

	connect(cnd, SIGNAL(newNameSet(QString)), this,
						SLOT(newNameSet(QString)));
	cnd->show();
}

void DeviceView::notifyCreation()
{
	KNotification *notification= new KNotification("device_added", this);
	notification->setTitle(i18n("New device"));
	notification->setText(i18n("Device %1 has been paired",
					properties["Alias"].toString()));
	notification->sendEvent();
}

void DeviceView::notifyDestruction()
{
	KNotification *notification= new KNotification("device_removed");
	notification->setTitle(i18n("Device deleted"));
	notification->setText(i18n("Device %1 has been unpaired",
					properties["Alias"].toString()));
	notification->sendEvent();
}
