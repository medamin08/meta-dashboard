import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: card
    property string label: ""
    property double value: 0
    property double minVal: 0
    property double maxVal: 100
    property string unit: ""
    property int    decimals: 0
    property color  arcColor: "#79c0ff"
    property double warningThreshold: maxVal * 0.85
    property bool   warnLow: false

    color: "#161b22"
    radius: 10
    border.color: isWarning ? Qt.rgba(1,0.3,0.2,0.6) : "#21262d"
    border.width: isWarning ? 2 : 1

    readonly property bool isWarning: warnLow
        ? value <= warningThreshold
        : value >= warningThreshold

    readonly property double pct: Math.max(0, Math.min(1, (value - minVal) / (maxVal - minVal)))
    readonly property color barColor: card.isWarning ? "#da3633" : card.arcColor

    // Warning flash
    SequentialAnimation on opacity {
        running: card.isWarning
        loops: Animation.Infinite
        NumberAnimation { to: 0.5; duration: 400 }
        NumberAnimation { to: 1.0; duration: 400 }
    }

    Column {
        anchors.centerIn: parent
        spacing: 6

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: card.label.toUpperCase()
            color: "#8b949e"
            font.pixelSize: 11
            font.letterSpacing: 1.5
        }

        // Thermometer Visual
        Item {
            width: 40
            height: 100
            anchors.horizontalCenter: parent.horizontalCenter

            // Stem background
            Rectangle {
                width: 12
                height: parent.height - 18
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                radius: width / 2
                color: "#21262d"
            }

            // Bulb background
            Rectangle {
                width: 28
                height: 28
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                radius: width / 2
                color: "#21262d"
            }

            // Fill Container
            Item {
                width: 12
                height: parent.height - 18 - 14 // minus top/bottom margins to inner fill
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 14 // middle of bulb
                anchors.horizontalCenter: parent.horizontalCenter

                Rectangle {
                    width: 8
                    height: parent.height * card.pct
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    radius: width / 2
                    color: card.barColor
                    
                    Behavior on height {
                        NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
                    }
                }
            }

            // Bulb fill
            Rectangle {
                width: 20
                height: 20
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 4
                anchors.horizontalCenter: parent.horizontalCenter
                radius: width / 2
                color: card.barColor
            }
            
            // Optional scale markings
            Column {
                anchors.left: parent.left
                anchors.leftMargin: -25
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 28
                anchors.topMargin: 4
                
                Item { height: 1; width: 8; Rectangle { anchors.right: parent.right; width: 6; height: 1; color: "#484f58" } }
                Item { height: (parent.height - 2) / 4; width: 8; Rectangle { anchors.right: parent.right; anchors.bottom: parent.bottom; width: 3; height: 1; color: "#484f58" } }
                Item { height: (parent.height - 2) / 4; width: 8; Rectangle { anchors.right: parent.right; anchors.bottom: parent.bottom; width: 6; height: 1; color: "#484f58" } }
                Item { height: (parent.height - 2) / 4; width: 8; Rectangle { anchors.right: parent.right; anchors.bottom: parent.bottom; width: 3; height: 1; color: "#484f58" } }
                Item { height: (parent.height - 2) / 4; width: 8; Rectangle { anchors.right: parent.right; anchors.bottom: parent.bottom; width: 6; height: 1; color: "#484f58" } }
            }
        }

        // Value readout
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: value.toFixed(card.decimals)
            color: card.isWarning ? "#f85149" : "white"
            font.pixelSize: 28
            font.bold: true
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: card.unit
            color: "#484f58"
            font.pixelSize: 12
        }

        // Min/max labels
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 140
            Text { text: card.minVal.toString(); color: "#484f58"; font.pixelSize: 10; width: parent.width/2 }
            Text { text: card.maxVal.toString(); color: "#484f58"; font.pixelSize: 10; width: parent.width/2; horizontalAlignment: Text.AlignRight }
        }
    }
}
