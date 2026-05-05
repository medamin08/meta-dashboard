#include "BatteryCanHandler.h"

BatteryCanHandler::BatteryCanHandler(QObject *parent) : QObject(parent)
{
    connectDevice();
}

BatteryCanHandler::~BatteryCanHandler()
{
    disconnectDevice();
}

double BatteryCanHandler::voltage() const
{
    return m_voltage;
}

void BatteryCanHandler::connectDevice()
{
    QString errorString;
    m_canDevice = QCanBus::instance()->createDevice("socketcan", "vcan0", &errorString);

    if (!m_canDevice) {
        qWarning() << "Error creating CAN device:" << errorString;
        return;
    }

    connect(m_canDevice, &QCanBusDevice::framesReceived, this, &BatteryCanHandler::processReceivedFrames);
    connect(m_canDevice, &QCanBusDevice::errorOccurred, this, &BatteryCanHandler::onErrorOccurred);

    if (m_canDevice->connectDevice()) {
        qDebug() << "BatteryCanHandler successfully connected to vcan0";
    } else {
        qWarning() << "BatteryCanHandler connection failed:" << m_canDevice->errorString();
    }
}

void BatteryCanHandler::disconnectDevice()
{
    if (!m_canDevice)
        return;

    m_canDevice->disconnectDevice();
    delete m_canDevice;
    m_canDevice = nullptr;
}

void BatteryCanHandler::processReceivedFrames()
{
    if (!m_canDevice) return;

    while (m_canDevice->framesAvailable()) {
        const QCanBusFrame frame = m_canDevice->readFrame();

        // Looking for standard OBD-II response ID 0x7E8
        if (frame.frameId() == 0x7E8) {
            QByteArray data = frame.payload();
            
            // Validate response payload size and headers
            // Expecting: [Length] [Mode+0x40=0x41] [PID=0x42] [Data A] [Data B]
            if (data.size() >= 5 && data.at(1) == 0x41 && data.at(2) == 0x42) {
                int a = static_cast<unsigned char>(data.at(3));
                int b = static_cast<unsigned char>(data.at(4));
                double newVoltage = (256 * a + b) / 1000.0;

                if (newVoltage != m_voltage) {
                    m_voltage = newVoltage;
                    emit voltageChanged();
                }
            }
        }
    }
}

void BatteryCanHandler::onStateChanged(QCanBusDevice::CanBusDeviceState state)
{
    qDebug() << "BatteryCanHandler state changed to:" << state;
}

void BatteryCanHandler::onErrorOccurred(QCanBusDevice::CanBusError error)
{
    qWarning() << "BatteryCanHandler error:" << m_canDevice->errorString() << "Code:" << error;
}
