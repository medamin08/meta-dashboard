#include "FuelCanHandler.h"

FuelCanHandler::FuelCanHandler(QObject *parent) : QObject(parent)
{
    connectDevice();
}

FuelCanHandler::~FuelCanHandler()
{
    disconnectDevice();
}

double FuelCanHandler::fuelLevel() const
{
    return m_fuelLevel;
}

void FuelCanHandler::connectDevice()
{
    QString errorString;
    m_canDevice = QCanBus::instance()->createDevice("socketcan", "vcan0", &errorString);

    if (!m_canDevice) {
        qWarning() << "Error creating CAN device:" << errorString;
        return;
    }

    connect(m_canDevice, &QCanBusDevice::framesReceived, this, &FuelCanHandler::processReceivedFrames);
    connect(m_canDevice, &QCanBusDevice::errorOccurred, this, &FuelCanHandler::onErrorOccurred);

    if (m_canDevice->connectDevice()) {
        qDebug() << "FuelCanHandler successfully connected to vcan0";
    } else {
        qWarning() << "FuelCanHandler connection failed:" << m_canDevice->errorString();
    }
}

void FuelCanHandler::disconnectDevice()
{
    if (!m_canDevice)
        return;

    m_canDevice->disconnectDevice();
    delete m_canDevice;
    m_canDevice = nullptr;
}

void FuelCanHandler::processReceivedFrames()
{
    if (!m_canDevice) return;

    while (m_canDevice->framesAvailable()) {
        const QCanBusFrame frame = m_canDevice->readFrame();

        if (frame.frameId() == 0x7E8) {
            QByteArray data = frame.payload();
            
            // Validate response payload size and headers
            // Expecting: [Length] [Mode+0x40=0x41] [PID=0x2F] [Data A]
            if (data.size() >= 4 && data.at(1) == 0x41 && data.at(2) == 0x2F) {
                int fuelVarA = static_cast<unsigned char>(data.at(3));
                double newFuel = (fuelVarA * 100.0) / 255.0;

                if (qAbs(newFuel - m_fuelLevel) > 0.1) {
                    m_fuelLevel = newFuel;
                    emit fuelLevelChanged();
                }
            }
        }
    }
}

void FuelCanHandler::onStateChanged(QCanBusDevice::CanBusDeviceState state)
{
    qDebug() << "FuelCanHandler state changed to:" << state;
}

void FuelCanHandler::onErrorOccurred(QCanBusDevice::CanBusError error)
{
    qWarning() << "FuelCanHandler error:" << m_canDevice->errorString() << "Code:" << error;
}
