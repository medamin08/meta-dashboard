import QtQuick
import QtQuick.Controls

Rectangle {
    property string label: ""
    property bool   active: true
    property color  baseColor: "#238636"
    width: 48; height: 18; radius: 9
    opacity: active ? 1.0 : 0.4
    color: Qt.rgba(
        Qt.color(baseColor).r,
        Qt.color(baseColor).g,
        Qt.color(baseColor).b, 0.2)
    border.color: baseColor
    border.width: 1
    Text {
        anchors.centerIn: parent
        text: label
        color: baseColor
        font.pixelSize: 10
        font.bold: true
    }
}
