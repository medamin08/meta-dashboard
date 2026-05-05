#ifndef DTCCANHANDLER_H
#define DTCCANHANDLER_H

#include <QObject>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>
#include <QVariantList>
#include <QVariantMap>
#include <QJsonObject>
#include <QStringList>

class DtcCanHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList activeDtcs READ activeDtcs NOTIFY activeDtcsChanged)

public:
    explicit DtcCanHandler(QObject *parent = nullptr);
    ~DtcCanHandler();

    QVariantList activeDtcs() const;

    Q_INVOKABLE void requestClearDtcs();
    Q_INVOKABLE void requestDtcs();

public slots:
    void connectDevice();
    void disconnectDevice();

signals:
    void activeDtcsChanged();

private slots:
    void processReceivedFrames();
    void onStateChanged(QCanBusDevice::CanBusDeviceState state);
    void onErrorOccurred(QCanBusDevice::CanBusError error);

private:
    void loadDtcDatabase();
    QString parseDtcBits(unsigned char a, unsigned char b);

    QCanBusDevice *m_canDevice = nullptr;
    QVariantList m_activeDtcs;
    QJsonObject m_dtcDatabase;
};

#endif // DTCCANHANDLER_H
