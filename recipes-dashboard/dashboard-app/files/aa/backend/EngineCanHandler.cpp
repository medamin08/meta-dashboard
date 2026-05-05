#include "EngineCanHandler.h"

EngineCanHandler::EngineCanHandler(QObject *parent) : QObject(parent)
{
    connectDevice();
}

EngineCanHandler::~EngineCanHandler()
{
    disconnectDevice();
}

int EngineCanHandler::engineTemp() const
{
    return m_engineTemp;
}

void EngineCanHandler::connectDevice()
{
    QString errorString;
    m_canDevice = QCanBus::instance()->createDevice("socketcan", "vcan0", &errorString);

    if (!m_canDevice) {
        qWarning() << "Error creating CAN device:" << errorString;
        return;
    }

    connect(m_canDevice, &QCanBusDevice::framesReceived, this, &EngineCanHandler::processReceivedFrames);
    connect(m_canDevice, &QCanBusDevice::errorOccurred, this, &EngineCanHandler::onErrorOccurred);

    if (m_canDevice->connectDevice()) {
        qDebug() << "EngineCanHandler successfully connected to vcan0";
    } else {
        qWarning() << "EngineCanHandler connection failed:" << m_canDevice->errorString();
    }
}

void EngineCanHandler::disconnectDevice()
{
    if (!m_canDevice)
        return;

    m_canDevice->disconnectDevice();
    delete m_canDevice;
    m_canDevice = nullptr;
}

void EngineCanHandler::processReceivedFrames()
{
    if (!m_canDevice) return;

    while (m_canDevice->framesAvailable()) {
        const QCanBusFrame frame = m_canDevice->readFrame();

        // Looking for standard OBD-II response ID 0x7E8
        if (frame.frameId() == 0x7E8) {
            QByteArray data = frame.payload();
            
            // Validate response payload size and headers
            // Expecting: [Length] [Mode+0x40=0x41] [PID=0x05] [Data A]
            if (data.size() >= 4 && data.at(1) == 0x41 && data.at(2) == 0x05) {
                int tempVarA = static_cast<unsigned char>(data.at(3));
                int newTemp = tempVarA - 40;

                if (newTemp != m_engineTemp) {
                    m_engineTemp = newTemp;
                    emit engineTempChanged();
                }
            }
        }
    }
}

void EngineCanHandler::onStateChanged(QCanBusDevice::CanBusDeviceState state)
{
    qDebug() << "EngineCanHandler state changed to:" << state;
}

void EngineCanHandler::onErrorOccurred(QCanBusDevice::CanBusError error)
{
    qWarning() << "EngineCanHandler error:" << m_canDevice->errorString() << "Code:" << error;
}
