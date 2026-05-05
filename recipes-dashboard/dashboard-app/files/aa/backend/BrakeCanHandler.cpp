#include "BrakeCanHandler.h"

BrakeCanHandler::BrakeCanHandler(QObject *parent) : QObject(parent)
{
    connectDevice();
}

BrakeCanHandler::~BrakeCanHandler()
{
    disconnectDevice();
}

double BrakeCanHandler::brakeFluidPressure() const
{
    return m_brakeFluidPressure;
}

void BrakeCanHandler::connectDevice()
{
    QString errorString;
    m_canDevice = QCanBus::instance()->createDevice("socketcan", "vcan0", &errorString);

    if (!m_canDevice) {
        qWarning() << "Error creating CAN device:" << errorString;
        return;
    }

    connect(m_canDevice, &QCanBusDevice::framesReceived, this, &BrakeCanHandler::processReceivedFrames);
    connect(m_canDevice, &QCanBusDevice::errorOccurred,  this, &BrakeCanHandler::onErrorOccurred);

    if (m_canDevice->connectDevice()) {
        qDebug() << "BrakeCanHandler successfully connected to vcan0";
    } else {
        qWarning() << "BrakeCanHandler connection failed:" << m_canDevice->errorString();
    }
}

void BrakeCanHandler::disconnectDevice()
{
    if (!m_canDevice)
        return;

    m_canDevice->disconnectDevice();
    delete m_canDevice;
    m_canDevice = nullptr;
}

void BrakeCanHandler::processReceivedFrames()
{
    if (!m_canDevice) return;

    while (m_canDevice->framesAvailable()) {
        const QCanBusFrame frame = m_canDevice->readFrame();

        // Looking for standard OBD-II response ID 0x7E8
        if (frame.frameId() == 0x7E8) {
            QByteArray data = frame.payload();

            // Expecting: [Length=0x04] [0x41] [PID=0x6D] [Data A] [Data B]
            // Pressure range: (256*A + B) * 0.03 kPa  →  /100 to get bar
            if (data.size() >= 5 && data.at(1) == 0x41 && data.at(2) == 0x6D) {
                int a = static_cast<unsigned char>(data.at(3));
                int b = static_cast<unsigned char>(data.at(4));
                double kPa = (256.0 * a + b) * 0.03;
                double bar = kPa / 100.0;

                if (bar != m_brakeFluidPressure) {
                    m_brakeFluidPressure = bar;
                    emit brakeFluidPressureChanged();
                }
            }
        }
    }
}

void BrakeCanHandler::onStateChanged(QCanBusDevice::CanBusDeviceState state)
{
    qDebug() << "BrakeCanHandler state changed to:" << state;
}

void BrakeCanHandler::onErrorOccurred(QCanBusDevice::CanBusError error)
{
    qWarning() << "BrakeCanHandler error:" << m_canDevice->errorString() << "Code:" << error;
}
