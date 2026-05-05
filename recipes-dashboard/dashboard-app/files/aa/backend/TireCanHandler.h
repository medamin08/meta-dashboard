#ifndef TIRECANHANDLER_H
#define TIRECANHANDLER_H

#include <QObject>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>
#include <QDebug>

class TireCanHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double tpFL READ tpFL NOTIFY tpFLChanged)
    Q_PROPERTY(double tpFR READ tpFR NOTIFY tpFRChanged)
    Q_PROPERTY(double tpRL READ tpRL NOTIFY tpRLChanged)
    Q_PROPERTY(double tpRR READ tpRR NOTIFY tpRRChanged)

public:
    explicit TireCanHandler(QObject *parent = nullptr);
    ~TireCanHandler();

    double tpFL() const;
    double tpFR() const;
    double tpRL() const;
    double tpRR() const;

public slots:
    void connectDevice();
    void disconnectDevice();

signals:
    void tpFLChanged();
    void tpFRChanged();
    void tpRLChanged();
    void tpRRChanged();

private slots:
    void processReceivedFrames();
    void onStateChanged(QCanBusDevice::CanBusDeviceState state);
    void onErrorOccurred(QCanBusDevice::CanBusError error);

private:
    QCanBusDevice *m_canDevice = nullptr;
    double m_tpFL = 2.3;
    double m_tpFR = 2.3;
    double m_tpRL = 2.2;
    double m_tpRR = 2.2;
};

#endif // TIRECANHANDLER_H
