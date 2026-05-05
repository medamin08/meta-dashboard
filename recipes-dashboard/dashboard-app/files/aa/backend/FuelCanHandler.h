#ifndef FUELCANHANDLER_H
#define FUELCANHANDLER_H

#include <QObject>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>
#include <QDebug>

class FuelCanHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double fuelLevel READ fuelLevel NOTIFY fuelLevelChanged)

public:
    explicit FuelCanHandler(QObject *parent = nullptr);
    ~FuelCanHandler();

    double fuelLevel() const;

public slots:
    void connectDevice();
    void disconnectDevice();

signals:
    void fuelLevelChanged();

private slots:
    void processReceivedFrames();
    void onStateChanged(QCanBusDevice::CanBusDeviceState state);
    void onErrorOccurred(QCanBusDevice::CanBusError error);

private:
    QCanBusDevice *m_canDevice = nullptr;
    double m_fuelLevel = 100.0;
};

#endif // FUELCANHANDLER_H
