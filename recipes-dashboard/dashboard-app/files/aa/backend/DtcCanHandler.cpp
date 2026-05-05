#include "DtcCanHandler.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <QCoreApplication>

DtcCanHandler::DtcCanHandler(QObject *parent) : QObject(parent)
{
    loadDtcDatabase();
    connectDevice();
}

DtcCanHandler::~DtcCanHandler()
{
    disconnectDevice();
}

QVariantList DtcCanHandler::activeDtcs() const
{
    return m_activeDtcs;
}

void DtcCanHandler::loadDtcDatabase()
{
    QFile file(QStringLiteral(":/Dashboard/dtcs.json"));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open dtcs.json database!";
        return;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        m_dtcDatabase = doc.object();
        qDebug() << "Loaded" << m_dtcDatabase.keys().size() << "DTC definitions.";
    }
}

void DtcCanHandler::connectDevice()
{
    QString errorString;
    m_canDevice = QCanBus::instance()->createDevice("socketcan", "vcan0", &errorString);

    if (!m_canDevice) {
        qWarning() << "Error creating CAN device:" << errorString;
        return;
    }

    connect(m_canDevice, &QCanBusDevice::framesReceived, this, &DtcCanHandler::processReceivedFrames);
    connect(m_canDevice, &QCanBusDevice::errorOccurred, this, &DtcCanHandler::onErrorOccurred);

    if (m_canDevice->connectDevice()) {
        qDebug() << "DtcCanHandler successfully connected to vcan0";
        // Request DTCs on startup
        requestDtcs();
    } else {
        qWarning() << "DtcCanHandler connection failed:" << m_canDevice->errorString();
    }
}

void DtcCanHandler::disconnectDevice()
{
    if (!m_canDevice) return;
    m_canDevice->disconnectDevice();
    delete m_canDevice;
    m_canDevice = nullptr;
}

QString DtcCanHandler::parseDtcBits(unsigned char a, unsigned char b)
{
    QString code;
    // Prefix (first 2 bits of A)
    int prefixBits = (a >> 6) & 0x03;
    switch (prefixBits) {
        case 0: code += "P"; break;
        case 1: code += "C"; break;
        case 2: code += "B"; break;
        case 3: code += "U"; break;
    }
    
    // Second char (next 2 bits of A)
    int secondChar = (a >> 4) & 0x03;
    code += QString::number(secondChar, 16).toUpper();
    
    // Third char (last 4 bits of A)
    int thirdChar = a & 0x0F;
    code += QString::number(thirdChar, 16).toUpper();
    
    // Fourth char (first 4 bits of B)
    int fourthChar = (b >> 4) & 0x0F;
    code += QString::number(fourthChar, 16).toUpper();
    
    // Fifth char (last 4 bits of B)
    int fifthChar = b & 0x0F;
    code += QString::number(fifthChar, 16).toUpper();
    
    return code;
}

void DtcCanHandler::processReceivedFrames()
{
    if (!m_canDevice) return;

    while (m_canDevice->framesAvailable()) {
        const QCanBusFrame frame = m_canDevice->readFrame();

        if (frame.frameId() == 0x7E8) {
            QByteArray data = frame.payload();
            
            // Expected payload size minimum for Mode 03: 
            // length byte + 0x43 (mode 3 response) + num dtcs + dtc bytes
            if (data.size() >= 3 && data.at(1) == 0x43) {
                int numDtcs = data.at(2);
                QVariantList newActiveDtcs;
                
                int dtcCountInFrame = (data.size() - 3) / 2;
                int countToParse = qMin(numDtcs, dtcCountInFrame);
                
                for (int i = 0; i < countToParse; ++i) {
                    unsigned char a = data.at(3 + (i * 2));
                    unsigned char b = data.at(4 + (i * 2));
                    
                    if (a == 0 && b == 0) continue; // padding
                    
                    QString code = parseDtcBits(a, b);
                    QString desc = "Unknown code";
                    if (m_dtcDatabase.contains(code)) {
                        desc = m_dtcDatabase.value(code).toString();
                    }
                    
                    QVariantMap dtcMap;
                    dtcMap["code"] = code;
                    dtcMap["desc"] = desc;
                    newActiveDtcs.append(dtcMap);
                }
                
                if (m_activeDtcs != newActiveDtcs) {
                    m_activeDtcs = newActiveDtcs;
                    emit activeDtcsChanged();
                }
            } 
            // Mode 04 response (clear DTCs successful) -> 0x44
            else if (data.size() >= 2 && data.at(1) == 0x44) {
                m_activeDtcs.clear();
                emit activeDtcsChanged();
            }
        }
    }
}

void DtcCanHandler::requestDtcs()
{
    if (!m_canDevice) return;
    QCanBusFrame frame(0x7DF, QByteArray::fromHex("0103000000000000"));
    m_canDevice->writeFrame(frame);
}

void DtcCanHandler::requestClearDtcs()
{
    if (!m_canDevice) return;
    QCanBusFrame frame(0x7DF, QByteArray::fromHex("0104000000000000"));
    m_canDevice->writeFrame(frame);
}

void DtcCanHandler::onStateChanged(QCanBusDevice::CanBusDeviceState state)
{
    qDebug() << "DtcCanHandler state changed to:" << state;
}

void DtcCanHandler::onErrorOccurred(QCanBusDevice::CanBusError error)
{
    qWarning() << "DtcCanHandler error:" << m_canDevice->errorString() << "Code:" << error;
}
