import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property string title: "PANEL"
    default property alias contentItem: container.data

    color: "#161b22"
    radius: 8
    border.color: "#30363d"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 8

        Text {
            text: root.title.toUpperCase()
            color: "#8b949e"
            font.pixelSize: 11
            font.bold: true
            font.letterSpacing: 1.5
            Layout.fillWidth: true
        }
        
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#30363d"
        }

        Item {
            id: container
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
