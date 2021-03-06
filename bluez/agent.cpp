/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -a agent.h:agent.cpp agent.xml
 *
 * qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "agent.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class AgentAdaptor
 */

AgentAdaptor::AgentAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

AgentAdaptor::~AgentAdaptor()
{
    // destructor
}

void AgentAdaptor::Authorize(const QDBusObjectPath &device, const QString &uuid)
{
    // handle method call org.bluez.Agent.Authorize
    QMetaObject::invokeMethod(parent(), "Authorize", Q_ARG(QDBusObjectPath, device), Q_ARG(QString, uuid));
}

void AgentAdaptor::Cancel()
{
    // handle method call org.bluez.Agent.Cancel
    QMetaObject::invokeMethod(parent(), "Cancel");
}

void AgentAdaptor::ConfirmModeChange(const QString &mode)
{
    // handle method call org.bluez.Agent.ConfirmModeChange
    QMetaObject::invokeMethod(parent(), "ConfirmModeChange", Q_ARG(QString, mode));
}

void AgentAdaptor::DisplayPasskey(const QDBusObjectPath &device, uint passkey)
{
    // handle method call org.bluez.Agent.DisplayPasskey
    QMetaObject::invokeMethod(parent(), "DisplayPasskey", Q_ARG(QDBusObjectPath, device), Q_ARG(uint, passkey));
}

void AgentAdaptor::Release()
{
    // handle method call org.bluez.Agent.Release
    QMetaObject::invokeMethod(parent(), "Release");
}

void AgentAdaptor::RequestConfirmation(const QDBusObjectPath &device, uint passkey)
{
    // handle method call org.bluez.Agent.RequestConfirmation
    QMetaObject::invokeMethod(parent(), "RequestConfirmation", Q_ARG(QDBusObjectPath, device), Q_ARG(uint, passkey));
}

uint AgentAdaptor::RequestPasskey(const QDBusObjectPath &device)
{
    // handle method call org.bluez.Agent.RequestPasskey
    uint out0;
    QMetaObject::invokeMethod(parent(), "RequestPasskey", Q_RETURN_ARG(uint, out0), Q_ARG(QDBusObjectPath, device));
    return out0;
}

QString AgentAdaptor::RequestPinCode(const QDBusObjectPath &device,
						const QDBusMessage &message)
{
    // handle method call org.bluez.Agent.RequestPinCode
    QString out0;
    QMetaObject::invokeMethod(parent(), "RequestPinCode",
						Q_RETURN_ARG(QString, out0),
						Q_ARG(QDBusObjectPath, device),
						Q_ARG(QDBusMessage, message));
    return out0;
}

