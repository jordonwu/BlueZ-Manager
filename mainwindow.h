#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <bluez/manager.h>
#include <adapterview.h>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:

public slots:

private:
    Manager manager;
    QList<AdapterView *> adapters;
};

#endif // MAINWINDOW_H