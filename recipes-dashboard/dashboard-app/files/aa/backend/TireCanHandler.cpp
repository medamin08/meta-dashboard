#include "TireCanHandler.h"

TireCanHandler::TireCanHandler(QObject *parent) : QObject(parent)
{
    connectDevice();
}

TireCanHandler::~TireCanHandler()
{
    disconnectDevice();
}

double TireCanHandler::tpFL() const { return m_tpFL; }
double TireCanHandler::tpFR() const { return m_tpFR; }
double TireCanHandler::tpRL() const { return m_tpRL; }
double TireCanHandler::tpRR() const { return m_tpRR; }

void TireCanHandler::connectDevice()
{
    QString errorString;
    m_canDevice = QCanBus::instance()->createDevice("socketcan", "vcan0", &errorString);

    if (!m_canDevice) {
        qWarning() << "Error creating CAN device:" << errorString;
        return;
    }

    connect(m_canDevice, &QCanBusDevice::framesReceived, this, &TireCanHandler::processReceivedFrames);
    connect(m_canDevice, &QCanBusDevice::errorOccurred, this, &TireCanHandler::onErrorOccurred);

    if (m_canDevice->connectDevice()) {
        qDebug() << "TireCanHandler successfully connected to vcan0";
    } else {
        qWarning() << "TireCanHandler connection failed:" << m_canDevice->errorString();
    }
}

void TireCanHandler::disconnectDevice()
{
    if (!m_canDevice)
        return;

    m_canDevice->disconnectDevice();
    delete m_canDevice;
    m_canDevice = nullptr;
}

void TireCanHandler::processReceivedFrames()
{
    if (!m_canDevice) return;

    while (m_canDevice->framesAvailable()) {
        const QCanBusFrame frame = m_canDevice->readFrame();

        // Looking for standard OBD-II response ID 0x7E8
        if (frame.frameId() == 0x7E8) {
            QByteArray data = frame.payload();
            
            // Validate response payload size and headers
            // Expecting: [Length] [Mode+0x40=0x41] [PID=0x50] [FL] [FR] [RL] [RR]
            if (data.size() >= 7 && data.at(1) == 0x41 && data.at(2) == 0x50) {
                double newFL = static_cast<unsigned char>(data.at(3)) / 100.0;
                double newFR = static_cast<unsigned char>(data.at(4)) / 100.0;
                double newRL = static_cast<unsigned char>(data.at(5)) / 100.0;
                double newRR = static_cast<unsigned char>(data.at(6)) / 100.0;

                if (!qFuzzyCompare(newFL, m_tpFL)) { m_tpFL = newFL; emit tpFLChanged(); }
                if (!qFuzzyCompare(newFR, m_tpFR)) { m_tpFR = newFR; emit tpFRChanged(); }
                if (!qFuzzyCompare(newRL, m_tpRL)) { m_tpRL = newRL; emit tpRLChanged(); }
                if (!qFuzzyCompare(newRR, m_tpRR)) { m_tpRR = newRR; emit tpRRChanged(); }
            }
        }
    }
}

void TireCanHandler::onStateChanged(QCanBusDevice::CanBusDeviceState state)
{
    qDebug() << "TireCanHandler state changed to:" << state;
}

void TireCanHandler::onErrorOccurred(QCanBusDevice::CanBusError error)
{
    qWarning() << "TireCanHandler error:" << m_canDevice->errorString() << "Code:" << error;
}
