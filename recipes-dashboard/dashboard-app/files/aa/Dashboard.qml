import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    width: 800
    height: 480
    title: "Dashboard UI"
    color: "#0d1117"
    visibility: Window.FullScreen

    // ── Keyboard shortcuts ──────────────────────────────────────────────
    Item {
        focus: true
        Keys.onEscapePressed: root.visibility = Window.Windowed
        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_F11) {
                root.visibility = (root.visibility === Window.FullScreen)
                    ? Window.Windowed
                    : Window.FullScreen
                event.accepted = true
            }
        }
    }

    // ── Simulated live data (pure QML, no C++ needed) ──────────────────
    QtObject {
        id: sim
        property double phase: 0
        property int mileage: 84320 + phase * 2
        property int serviceDue: 85000
    }

    // Removed static dtcModel

    Timer {
        interval: 50
        running: true
        repeat: true
        onTriggered: sim.phase += 0.04
    }

    // ── Top status bar ──────────────────────────────────────────────────
    Rectangle {
        id: statusBar
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 36
        color: "#161b22"

        Row {
            anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
            spacing: 20

            StatusPill { label: "CAN"; active: true;  baseColor: "#238636" }
            StatusPill { label: "ECU"; active: true;  baseColor: "#238636" }
            StatusPill { label: "ABS"; active: false; baseColor: "#da3633" }
        }

        Text {
            anchors.centerIn: parent
            text: "CAN Diagnostic Dashboard"
            color: "#8b949e"
            font.pixelSize: 13
            font.letterSpacing: 2
        }

        Row {
            anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
            spacing: 12

            Text {
                text: Qt.formatTime(new Date(), "hh:mm:ss")
                color: "#8b949e"
                font.pixelSize: 12
                Timer { interval: 1000; running: true; repeat: true; onTriggered: parent.text = Qt.formatTime(new Date(), "hh:mm:ss") }
            }

            // ── Window controls (visible in fullscreen) ─────────────────
            Row {
                spacing: 6
                anchors.verticalCenter: parent.verticalCenter

                // Minimise
                Rectangle {
                    id: btnMinimise
                    width: 22; height: 22; radius: 11
                    color: minimiseHover.containsMouse ? "#444c56" : "#30363d"
                    Behavior on color { ColorAnimation { duration: 120 } }
                    Text {
                        anchors.centerIn: parent
                        text: "–"
                        color: "#e6edf3"
                        font.pixelSize: 14
                        font.bold: true
                    }
                    HoverHandler { id: minimiseHover }
                    TapHandler { onTapped: root.showMinimized() }
                }

                // Toggle fullscreen / restore
                Rectangle {
                    id: btnRestore
                    width: 22; height: 22; radius: 11
                    color: restoreHover.containsMouse ? "#444c56" : "#30363d"
                    Behavior on color { ColorAnimation { duration: 120 } }
                    Text {
                        anchors.centerIn: parent
                        text: root.visibility === Window.FullScreen ? "❐" : "⛶"
                        color: "#e6edf3"
                        font.pixelSize: 11
                    }
                    HoverHandler { id: restoreHover }
                    TapHandler {
                        onTapped: root.visibility = (root.visibility === Window.FullScreen)
                            ? Window.Windowed : Window.FullScreen
                    }
                }

                // Close
                Rectangle {
                    id: btnClose
                    width: 22; height: 22; radius: 11
                    color: closeHover.containsMouse ? "#da3633" : "#30363d"
                    Behavior on color { ColorAnimation { duration: 120 } }
                    Text {
                        anchors.centerIn: parent
                        text: "✕"
                        color: "#e6edf3"
                        font.pixelSize: 11
                        font.bold: true
                    }
                    HoverHandler { id: closeHover }
                    TapHandler { onTapped: Qt.quit() }
                }
            }
        }
    }

    // ── Main UI Layout ──────────────────────────────────────────────────
    RowLayout {
        anchors {
            top: statusBar.bottom
            left: parent.left; right: parent.right; bottom: parent.bottom
            margins: 12
        }
        spacing: 12

        // Left Column: Vitals & Maintenance
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: Math.floor(parent.width * 0.28)
            spacing: 12

            ThermometerCard {
                id: engTempCard
                label: "Eng Temp"
                value: engineBackend.engineTemp
                minVal: 60; maxVal: 130
                unit: "°C"
                arcColor: "#ffa657"
                warningThreshold: 110
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            GaugeCard {
                id: fuelCard
                label: "Fuel"
                value: fuelBackend.fuelLevel
                minVal: 0; maxVal: 100
                unit: "%"
                arcColor: "#3fb950"
                warningThreshold: 10
                warnLow: true
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            
            // Maintenance
            DiagPanel {
                title: "Maintenance"
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 4
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Odometer:"; color: "#8b949e"; font.pixelSize: 12 }
                        Item { Layout.fillWidth: true }
                        Text { text: Math.floor(sim.mileage) + " km"; color: "#e6edf3"; font.pixelSize: 14; font.bold: true }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Next Service:"; color: "#8b949e"; font.pixelSize: 12 }
                        Item { Layout.fillWidth: true }
                        Text { text: Math.floor(Math.max(0, sim.serviceDue - sim.mileage)) + " km"; color: (sim.serviceDue - sim.mileage < 1000) ? "#ffa657" : "#3fb950"; font.pixelSize: 14; font.bold: true }
                    }
                }
            }
        }

        // Right Area: Center (Tire Pressure) + Right (Battery/Brake) + Bottom (DTCs)
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12

            // Top section of Right Area (Tire + Battery/Brake)
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 200
                spacing: 12

                // ── Tire Pressure panel ──
                DiagPanel {
                    title: "Tire Pressure"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: Math.floor(root.width * 0.44)

                    Item {
                    anchors.fill: parent

                    // ── Car top-view image ──
                    Image {
                        id: carImgCenter
                        source: "car.png"
                        anchors.centerIn: parent
                        width: parent.width * 0.38
                        height: parent.height * 0.90
                        fillMode: Image.PreserveAspectFit
                        opacity: 0.85
                    }

                    // ── FL (front-left) ─────────────────────────────────────
                    Column {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.topMargin: 4
                        spacing: 0
                        Text {
                            text: "FL"
                            color: "#8b949e"
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 1
                        }
                        Row {
                            spacing: 2
                            property double tpFLValue: tireBackend.tpFL
                            Text { id: flV; text: parent.tpFLValue.toFixed(2); color: parent.tpFLValue < 2.0 ? "#f85149" : "#e6edf3"; font.pixelSize: 26; font.bold: true }
                            Text { text: "bar"; color: "#8b949e"; font.pixelSize: 11; anchors.baseline: flV.baseline }
                        }
                    }

                    // ── FR (front-right) ────────────────────────────────────
                    Column {
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.topMargin: 4
                        spacing: 0
                        Row {
                            anchors.right: parent.right
                            spacing: 2
                            property double tpFRValue: tireBackend.tpFR
                            Text { id: frV; text: parent.tpFRValue.toFixed(2); color: parent.tpFRValue < 2.0 ? "#f85149" : "#e6edf3"; font.pixelSize: 26; font.bold: true }
                            Text { text: "bar"; color: "#8b949e"; font.pixelSize: 11; anchors.baseline: frV.baseline }
                        }
                        Text {
                            text: "FR"
                            color: "#8b949e"
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 1
                            anchors.right: parent.right
                        }
                    }

                    // ── RL (rear-left) ──────────────────────────────────────
                    Column {
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        spacing: 0
                        Row {
                            spacing: 2
                            property double tpRLValue: tireBackend.tpRL
                            Text { id: rlV; text: parent.tpRLValue.toFixed(2); color: parent.tpRLValue < 2.0 ? "#f85149" : "#e6edf3"; font.pixelSize: 26; font.bold: true }
                            Text { text: "bar"; color: "#8b949e"; font.pixelSize: 11; anchors.baseline: rlV.baseline }
                        }
                        Text {
                            text: "RL"
                            color: "#8b949e"
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 1
                        }
                    }

                    // ── RR (rear-right) ─────────────────────────────────────
                    Column {
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        spacing: 0
                        Row {
                            anchors.right: parent.right
                            spacing: 2
                            property double tpRRValue: tireBackend.tpRR
                            Text { id: rrV; text: parent.tpRRValue.toFixed(2); color: parent.tpRRValue < 2.0 ? "#f85149" : "#e6edf3"; font.pixelSize: 26; font.bold: true }
                            Text { text: "bar"; color: "#8b949e"; font.pixelSize: 11; anchors.baseline: rrV.baseline }
                        }
                        Text {
                            text: "RR"
                            color: "#8b949e"
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 1
                            anchors.right: parent.right
                        }
                    }
                }
            }

                // Right Column: Electrical & DTCs
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: Math.floor(root.width * 0.28)
                    spacing: 12

                    BatteryCard {
                        label: "Battery"
                        value: batteryBackend.voltage
                        minVal: 11.5
                        maxVal: 14.5
                        fillColor: "#a371f7"
                        warningThreshold: 12.0
                        warnLow: true
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                    GaugeCard {
                        label: "Brake Fluid"
                        value: brakeBackend.brakeFluidPressure
                        decimals: 1
                        minVal: 0; maxVal: 100
                        unit: "bar"
                        arcColor: "#d2a8ff"
                        warningThreshold: 85
                        warnLow: false
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }

            // ── Active DTCs panel — full right-area width ─────────────────
            DiagPanel {
                title: "Active DTCs"
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 120

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: dtcBackend.activeDtcs
                        clip: true
                        spacing: 4
                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 28
                            color: index % 2 === 0 ? "#0d1117" : "transparent"
                            radius: 4
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 4
                                anchors.rightMargin: 4
                                spacing: 8
                                Rectangle {
                                    Layout.alignment: Qt.AlignVCenter
                                    width: 7; height: 7; radius: 4
                                    color: "#f85149"
                                }
                                Text {
                                    text: modelData.code
                                    color: "#ffa657"
                                    font.bold: true
                                    font.pixelSize: 13
                                    font.family: "monospace"
                                    Layout.preferredWidth: 70
                                }
                                Text {
                                    text: modelData.desc
                                    color: "#c9d1d9"
                                    font.pixelSize: 12
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        Item { Layout.fillWidth: true }
                        
                        Rectangle {
                            Layout.preferredWidth: 120
                            height: 28
                            radius: 4
                            color: updateHover.containsMouse ? "#2ea043" : "#238636"
                            Text {
                                anchors.centerIn: parent
                                text: "Update Faults"
                                color: "#ffffff"
                                font.pixelSize: 12
                                font.bold: true
                            }
                            HoverHandler { id: updateHover }
                            TapHandler {
                                onTapped: dtcBackend.requestDtcs()
                            }
                        }
                        
                        Rectangle {
                            Layout.preferredWidth: 120
                            height: 28
                            radius: 4
                            color: clearHover.containsMouse ? "#f85149" : "#da3633"
                            Text {
                                anchors.centerIn: parent
                                text: "Clear Faults"
                                color: "#ffffff"
                                font.pixelSize: 12
                                font.bold: true
                            }
                            HoverHandler { id: clearHover }
                            TapHandler {
                                onTapped: dtcBackend.requestClearDtcs()
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                    }
                }
            }
        }
    }

}