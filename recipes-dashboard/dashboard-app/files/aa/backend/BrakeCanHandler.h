#ifndef BRAKECANHANDLER_H
#define BRAKECANHANDLER_H

#include <QObject>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>
#include <QDebug>

class BrakeCanHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double brakeFluidPressure READ brakeFluidPressure NOTIFY brakeFluidPressureChanged)

public:
    explicit BrakeCanHandler(QObject *parent = nullptr);
    ~BrakeCanHandler();

    double brakeFluidPressure() const;

public slots:
    void connectDevice();
    void disconnectDevice();

signals:
    void brakeFluidPressureChanged();

private slots:
    void processReceivedFrames();
    void onStateChanged(QCanBusDevice::CanBusDeviceState state);
    void onErrorOccurred(QCanBusDevice::CanBusError error);

private:
    QCanBusDevice *m_canDevice = nullptr;
    double m_brakeFluidPressure = 0.0; // bar
};

#endif // BRAKECANHANDLER_H
