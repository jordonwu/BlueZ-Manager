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

#include "adapterview.h"
#include "ui_adapterview.h"

#include "bluez/adapter.h"
#include "bluez/agent.h"

enum {
	HIDDEN = 0,
	ALWAYS = 1,
	TEMPORAL = 2,
};

AdapterView::AdapterView(const QString path, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AdapterView),
	adapter("org.bluez", path, QDBusConnection::systemBus())
{
	ui->setupUi(this);

	QDBusPendingReply<QVariantMap> dprops = adapter.GetProperties();
	dprops.waitForFinished();

	if (!dprops.isValid()) {
		qCritical() << "Received unvalid reply";
		return;
	}

	QVariantMap props = dprops.value();

	ui->nameEdit->setText(props["Name"].toString());
	ui->powered->setChecked(props["Powered"].toBool());
	setAddress(props["Address"].toString());

	setVisibility(props["Discoverable"].toBool(),
					props["DiscoverableTimeout"].toInt());

	connect(ui->apply, SIGNAL(clicked()), this, SLOT(applyClicked()));
	connect(ui->powered, SIGNAL(clicked()), this, SLOT(poweredClicked()));
	connect(ui->visibility, SIGNAL(currentIndexChanged(int)), this,
						SLOT(comboChanged(int)));
	connect(&adapter, SIGNAL(PropertyChanged(QString,QDBusVariant)), this,
				SLOT(propertyChanged(QString,QDBusVariant)));
	connect(ui->timeout, SIGNAL(valueChanged(int)), this,
						SLOT(sliderChanged(int)));
	connect(&adapter, SIGNAL(DeviceRemoved(QDBusObjectPath)), this,
					SLOT(deviceRemoved(QDBusObjectPath)));
	connect(&adapter, SIGNAL(DeviceCreated(QDBusObjectPath)), this,
					SLOT(deviceCreated(QDBusObjectPath)));
	connect(ui->showDevices, SIGNAL(clicked(bool)), this,
						SLOT(showDevicesClicked(bool)));

	createDevicesView(qdbus_cast<QStringList>(props["Devices"]),
						props["Name"].toString());

	QString adapterPath = AGENT_BASE;
	adapterPath.append(path.split("/").last());

	new AgentAdaptor(this);
	QDBusConnection::systemBus().registerObject(adapterPath, this);

	// Do not check the response, if fails is assumed that other agent
	// is already registered an no option of register a new one.
	adapter.RegisterAgent(QDBusObjectPath(adapterPath), "DisplayYesNo");
}

AdapterView::~AdapterView()
{
	delete ui;
}

void AdapterView::createDevicesView(QStringList devicesPaths, QString name)
{
	devicesWindow = new DevicesWindow(this);
	foreach (QString path, devicesPaths)
		this->deviceCreated(QDBusObjectPath(path));

	devicesWindow->setWindowTitle(tr("Devices for adapter ") + name);
}

void AdapterView::setVisibility(bool visible, int timeout)
{
	this->timeout = timeout;
	ui->timeout->setValue(timeout);
	sliderChanged(timeout);

	if (visible) {
		if (timeout > 0) {
			ui->visibility->setCurrentIndex(TEMPORAL);
			ui->visibilityLabel->setEnabled(TRUE);
			ui->visibilityTime->setEnabled(TRUE);
			ui->timeout->setEnabled(TRUE);
			return;
		} else {
			ui->visibility->setCurrentIndex(ALWAYS);
		}
	} else {
		ui->visibility->setCurrentIndex(HIDDEN);
	}

	ui->visibilityLabel->setEnabled(FALSE);
	ui->visibilityTime->setEnabled(FALSE);
	ui->timeout->setEnabled(FALSE);
}

void AdapterView::propertyChanged(const QString name, const QDBusVariant value)
{
	if (name == "Name") {
		ui->nameEdit->setText(value.variant().toString());
		devicesWindow->setWindowTitle(tr("Devices for adapter ") +
						value.variant().toString());
	} else if (name == "Powered") {
		ui->powered->setChecked(value.variant().toBool());
	} else if (name == "Address") {
		setAddress(value.variant().toString());
	} else if (name == "Discoverable") {
		setVisibility(value.variant().toBool(), timeout);
	} else if (name == "DiscoverableTimeout") {
		setVisibility(ui->visibility->currentIndex() != HIDDEN,
						value.variant().toInt());
	}
}

void AdapterView::applyClicked()
{
	QString name = ui->nameEdit->text();

	adapter.SetProperty("Name", QDBusVariant(name));

	bool discoverable = ui->visibility->currentIndex() != HIDDEN;
	adapter.SetProperty("Discoverable", QDBusVariant(discoverable));

	if (ui->visibility->currentIndex() == TEMPORAL)
		adapter.SetProperty("DiscoverableTimeout",
			QDBusVariant((unsigned int) ui->timeout->value()));
	else
		adapter.SetProperty("DiscoverableTimeout",
						QDBusVariant((unsigned int)0));
}

void AdapterView::poweredClicked()
{
	adapter.SetProperty("Powered", QDBusVariant(ui->powered->isChecked()));
}

void AdapterView::setAddress(QString address)
{
	ui->header->setTitle(tr("Adapter") + " (" + address + ")");
}

OrgBluezAdapterInterface *AdapterView::getAdapter()
{
	return &adapter;
}

void AdapterView::sliderChanged(int value)
{
	QString text = QString::number(value);

	if (value == 1)
		text.append(tr("second"));
	else
		text.append(tr("seconds"));

	ui->visibilityTime->setText(text);
}

void AdapterView::comboChanged(int value)
{
	switch (value) {
	case HIDDEN:
	case ALWAYS:
		ui->visibilityLabel->setEnabled(FALSE);
		ui->visibilityTime->setEnabled(FALSE);
		ui->timeout->setEnabled(FALSE);
		break;
	case TEMPORAL:
		ui->visibilityLabel->setEnabled(TRUE);
		ui->visibilityTime->setEnabled(TRUE);
		ui->timeout->setEnabled(TRUE);
		if (ui->timeout->value() <= 1) {
			ui->timeout->setValue(1);
			sliderChanged(1);
		}
		break;
	default:
		break;
	}
}

QString AdapterView::adapterPath()
{
	return adapter.path();
}

DeviceView *AdapterView::getDeviceView(const QString path)
{
	foreach (DeviceView *view, devices) {
		if (view->devicePath() == path)
			return view;
	}

	return NULL;
}

void AdapterView::deviceRemoved(const QDBusObjectPath &device)
{
	DeviceView *view = getDeviceView(device.path());
	devices.removeAll(view);

	devicesWindow->removeWidget(view);
	delete view;
}

void AdapterView::deviceCreated(const QDBusObjectPath &device)
{
	DeviceView *deviceView = new DeviceView(device.path(), this);

	devicesWindow->addWidget(deviceView);
	devices.append(deviceView);

	connect(deviceView, SIGNAL(deletePairing(QString)), this,
						SLOT(deleteDevice(QString)));
}

void AdapterView::showDevicesClicked(bool checked)
{
	if (checked) {
		ui->showDevices->setText(tr("Hide devices"));
		devicesWindow->show();
		connect(devicesWindow, SIGNAL(finished(int)), ui->showDevices,
								SLOT(click()));
	} else {
		ui->showDevices->setText(tr("Show devices"));
		devicesWindow->hide();
	}
}

void AdapterView::deleteDevice(const QString &path)
{
	adapter.RemoveDevice(QDBusObjectPath(path));
}

void AdapterView::requestDiscovery()
{
	//TODO: emit signal when finished
	adapter.StartDiscovery();
}

/* D-Bus Agent slots */
void AdapterView::Authorize(const QDBusObjectPath &device, const QString &uuid)
{
	/* TODO: Implement this method */
	qDebug() << "Authorize received. Device: " << device.path() <<
							" Uuid: " << uuid;
}

void AdapterView::Cancel()
{
	/* TODO: Implement this method */
	qDebug() << "Cancel received";
}

void AdapterView::ConfirmModeChange(const QString &mode)
{
	/* TODO: Implement this method */
	qDebug() << "ConfirmModeChange received. Mode " << mode;
}

void AdapterView::DisplayPasskey(const QDBusObjectPath &device, uint passkey)
{
	/* TODO: Implement this method */
	qDebug() << "DisplayPasskey received. Device: " << device.path() <<
				" Passkey: " << QString::number(passkey);
}

void AdapterView::Release()
{
	/* TODO: Implement this method */
	qDebug() << "Release received";
}

void AdapterView::RequestConfirmation(const QDBusObjectPath &device,
								uint passkey)
{
	/* TODO: Implement this method */
	qDebug() << "RequestConfirmation received. Device: " << device.path() <<
				" Passkey: " << QString::number(passkey);
}

uint AdapterView::RequestPasskey(const QDBusObjectPath &device)
{
	/* TODO: Implement this method */
	qDebug() << "RequestPasskey received. Device " << device.path();
	return 0;
}

QString AdapterView::RequestPinCode(const QDBusObjectPath &device)
{
	/* TODO: Implement this method */
	qDebug() << "RequestPinCode received. Device: " << device.path();
	return "0000";
}
