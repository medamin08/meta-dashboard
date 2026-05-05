import QtQuick
import QtQuick.Controls

Row {
    property string label: ""
    property string value: "0000"
    spacing: 6
    Text { text: label; color: "#484f58"; font.pixelSize: 11; font.family: "monospace" }
    Text { text: value; color: "#8b949e"; font.pixelSize: 11; font.family: "monospace"; font.bold: true }
}
