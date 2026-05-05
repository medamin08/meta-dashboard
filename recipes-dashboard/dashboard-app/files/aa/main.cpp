#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QProcess>
#include "backend/EngineCanHandler.h"
#include "backend/FuelCanHandler.h"
#include "backend/BatteryCanHandler.h"
#include "backend/BrakeCanHandler.h"
#include "backend/TireCanHandler.h"
#include "backend/DtcCanHandler.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Launch the Python simulators in the background
    QProcess tempSimulatorProcess;
    tempSimulatorProcess.setProcessChannelMode(QProcess::ForwardedChannels); // Output directly to creator logs
    tempSimulatorProcess.start("python3", { QStringLiteral("/usr/share/dashboard/sim/temp_simulator.py") });

    QProcess fuelSimulatorProcess;
    fuelSimulatorProcess.setProcessChannelMode(QProcess::ForwardedChannels); 
    fuelSimulatorProcess.start("python3", { QStringLiteral("/usr/share/dashboard/sim/fuel_simulator.py") });

    QProcess batterySimulatorProcess;
    batterySimulatorProcess.setProcessChannelMode(QProcess::ForwardedChannels); 
    batterySimulatorProcess.start("python3", { QStringLiteral("/usr/share/dashboard/sim/battery_simulator.py") });

    QProcess brakeSimulatorProcess;
    brakeSimulatorProcess.setProcessChannelMode(QProcess::ForwardedChannels);
    brakeSimulatorProcess.start("python3", { QStringLiteral("/usr/share/dashboard/sim/brake_simulator.py") });

    QProcess tireSimulatorProcess;
    tireSimulatorProcess.setProcessChannelMode(QProcess::ForwardedChannels);
    tireSimulatorProcess.start("python3", { QStringLiteral("/usr/share/dashboard/sim/tire_simulator.py") });

    QProcess dtcSimulatorProcess;
    dtcSimulatorProcess.setProcessChannelMode(QProcess::ForwardedChannels);
    dtcSimulatorProcess.start("python3", { QStringLiteral("/usr/share/dashboard/sim/dtc_simulator.py") });

    EngineCanHandler engineBackend;
    FuelCanHandler fuelBackend;
    BatteryCanHandler batteryBackend;
    BrakeCanHandler brakeBackend;
    TireCanHandler tireBackend;
    DtcCanHandler dtcBackend;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("engineBackend", &engineBackend);
    engine.rootContext()->setContextProperty("fuelBackend", &fuelBackend);
    engine.rootContext()->setContextProperty("batteryBackend", &batteryBackend);
    engine.rootContext()->setContextProperty("brakeBackend", &brakeBackend);
    engine.rootContext()->setContextProperty("tireBackend", &tireBackend);
    engine.rootContext()->setContextProperty("dtcBackend", &dtcBackend);

    engine.load(QUrl(QStringLiteral("qrc:/Dashboard/Dashboard.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}