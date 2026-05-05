#ifndef BATTERYCANHANDLER_H
#define BATTERYCANHANDLER_H

#include <QObject>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>
#include <QDebug>

class BatteryCanHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double voltage READ voltage NOTIFY voltageChanged)

public:
    explicit BatteryCanHandler(QObject *parent = nullptr);
    ~BatteryCanHandler();

    double voltage() const;

public slots:
    void connectDevice();
    void disconnectDevice();

signals:
    void voltageChanged();

private slots:
    void processReceivedFrames();
    void onStateChanged(QCanBusDevice::CanBusDeviceState state);
    void onErrorOccurred(QCanBusDevice::CanBusError error);

private:
    QCanBusDevice *m_canDevice = nullptr;
    double m_voltage = 12.0;
};

#endif // BATTERYCANHANDLER_H
