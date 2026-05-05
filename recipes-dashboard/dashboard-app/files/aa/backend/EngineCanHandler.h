#ifndef ENGINECANHANDLER_H
#define ENGINECANHANDLER_H

#include <QObject>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>
#include <QDebug>

class EngineCanHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int engineTemp READ engineTemp NOTIFY engineTempChanged)

public:
    explicit EngineCanHandler(QObject *parent = nullptr);
    ~EngineCanHandler();

    int engineTemp() const;

public slots:
    void connectDevice();
    void disconnectDevice();

signals:
    void engineTempChanged();

private slots:
    void processReceivedFrames();
    void onStateChanged(QCanBusDevice::CanBusDeviceState state);
    void onErrorOccurred(QCanBusDevice::CanBusError error);

private:
    QCanBusDevice *m_canDevice = nullptr;
    int m_engineTemp = 70; // Default or disconnected state
};

#endif // ENGINECANHANDLER_H
