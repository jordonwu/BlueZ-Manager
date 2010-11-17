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

#include "deviceview.h"
#include "ui_deviceview.h"

DeviceView::DeviceView(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceView),
    device(path)
{
    ui->setupUi(this);

    QVariantMap props = device.getProperties();

    ui->device->setTitle(props["Alias"].toString());
    ui->realName->setText(props["Name"].toString());
    ui->address->setText(props["Address"].toString());
    setConnection(props["Connected"].toBool());

    connect(&device, SIGNAL(propertyChanged(QString,QVariant)),
	    this, SLOT(propertyChanged(QString, QVariant)));

    connect(ui->checkBox, SIGNAL(clicked()), this,
						SLOT(checkBoxClicked()));
}

DeviceView::~DeviceView()
{
    delete ui;
}

void DeviceView::setConnection(bool connected)
{
	ui->checkBox->setChecked(connected);
	ui->checkBox->setEnabled(connected);
	if (connected)
		ui->checkBox->setToolTip(tr("Connected"));
	else
		ui->checkBox->setToolTip(tr("Disconnected"));
}

QString DeviceView::devicePath()
{
	return device.getPath();
}

void DeviceView::propertyChanged(QString name, QVariant value)
{
	if (name == "Connected") {
		setConnection(value.toBool());
	} else if (name == "Alias") {
		ui->device->setTitle(value.toString());
	} else if (name == "Name") {
		ui->realName->setText(value.toString());
	}
}

void DeviceView::checkBoxClicked()
{
	if (!ui->checkBox->isEnabled())
		return;

	// Is only enabled if is already connected.
	ui->checkBox->setCheckState(Qt::PartiallyChecked);
	ui->checkBox->setToolTip(tr("Disconnecting"));
	ui->checkBox->setEnabled(FALSE);

	device.disconnect();
}
