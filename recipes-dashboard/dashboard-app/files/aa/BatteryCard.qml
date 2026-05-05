import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: card
    property string label: "BATTERY"
    property double value: 0
    property double minVal: 11.5
    property double maxVal: 14.5
    property color  fillColor: "#a371f7"
    property double warningThreshold: 12.0
    property bool   warnLow: true

    color: "#161b22"
    radius: 10
    border.color: isWarning ? Qt.rgba(1,0.3,0.2,0.6) : "#21262d"
    border.width: isWarning ? 2 : 1

    readonly property bool isWarning: warnLow
        ? value <= warningThreshold
        : value >= warningThreshold

    readonly property double pct: Math.max(0, Math.min(1, (value - minVal) / (maxVal - minVal)))

    // Warning flash
    SequentialAnimation on opacity {
        running: card.isWarning
        loops: Animation.Infinite
        NumberAnimation { to: 0.5; duration: 400 }
        NumberAnimation { to: 1.0; duration: 400 }
    }

    Column {
        anchors.centerIn: parent
        spacing: 12

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: card.label.toUpperCase()
            color: "#8b949e"
            font.pixelSize: 11
            font.letterSpacing: 1.5
        }

        // Battery Icon / Shape
        Item {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 100
            height: 44

            // Battery body
            Rectangle {
                id: batteryBody
                width: parent.width - 6
                height: parent.height
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                color: "transparent"
                border.color: card.isWarning ? "#da3633" : "#8b949e"
                border.width: 2
                radius: 6

                // Inner fill
                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.margins: 4
                    width: (parent.width - 8) * card.pct
                    radius: 3
                    color: card.isWarning ? "#da3633" : card.fillColor
                     
                    Behavior on width {
                        NumberAnimation { duration: 250; easing.type: Easing.OutSine }
                    }
                }
            }

            // Battery terminal
            Rectangle {
                width: 6
                height: 16
                anchors.left: batteryBody.right
                anchors.leftMargin: -1
                anchors.verticalCenter: parent.verticalCenter
                color: card.isWarning ? "#da3633" : "#8b949e"
                radius: 2
            }
        }

        // Percentage Readout
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: Math.round(card.pct * 100) + "%"
            color: card.isWarning ? "#f85149" : "white"
            font.pixelSize: 28
            font.bold: true
        }

        // Voltage readout (for extra info like the original had)
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: value.toFixed(1) + " V"
            color: "#484f58"
            font.pixelSize: 12
        }
    }
}
