cmake_minimum_required(VERSION 3.16)
project(qt-dashboard-ui-test VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick SerialBus)

qt_add_executable(qt-dashboard-ui-test
    main.cpp
    backend/EngineCanHandler.cpp
    backend/EngineCanHandler.h
    backend/FuelCanHandler.cpp
    backend/FuelCanHandler.h
    backend/BatteryCanHandler.cpp
    backend/BatteryCanHandler.h
    backend/BrakeCanHandler.cpp
    backend/BrakeCanHandler.h
    backend/TireCanHandler.cpp
    backend/TireCanHandler.h
    backend/DtcCanHandler.cpp
    backend/DtcCanHandler.h
)

target_compile_definitions(qt-dashboard-ui-test PRIVATE
    SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

qt_add_qml_module(qt-dashboard-ui-test
    URI Dashboard
    VERSION 1.0
    QML_FILES
        Dashboard.qml
        StatusPill.qml
        RawReadout.qml
        GaugeCard.qml
        DiagPanel.qml
        ThermometerCard.qml
        BatteryCard.qml
    RESOURCES
        car.png
        dtcs.json
)

target_link_libraries(qt-dashboard-ui-test PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::SerialBus
)
