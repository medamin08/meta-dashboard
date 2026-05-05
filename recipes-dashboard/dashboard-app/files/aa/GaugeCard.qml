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

        // Arc canvas
        Canvas {
            id: gaugeCanvas
            width: Math.min(160, card.width - 20)
            height: width * 0.625
            anchors.horizontalCenter: parent.horizontalCenter

            readonly property double pct: card.pct
            readonly property color  arc: card.isWarning ? "#da3633" : card.arcColor
            onPctChanged: requestPaint()
            onArcChanged: requestPaint()
            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()

            onPaint: {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);
                var cx = width / 2, cy = height - width * 0.05, r = (width / 2) * 0.9;
                var startAngle = Math.PI;
                var endAngle   = 2 * Math.PI;
                var lw = Math.max(2, width * 0.0625);

                // Track
                ctx.beginPath();
                ctx.arc(cx, cy, r, startAngle, endAngle);
                ctx.strokeStyle = "#21262d";
                ctx.lineWidth = lw;
                ctx.lineCap = "round";
                ctx.stroke();

                // Fill
                if (pct > 0) {
                    ctx.beginPath();
                    ctx.arc(cx, cy, r, startAngle, startAngle + pct * Math.PI);
                    ctx.strokeStyle = arc;
                    ctx.lineWidth = lw;
                    ctx.lineCap = "round";
                    ctx.stroke();
                }

                // Needle tip dot
                var needleAngle = startAngle + pct * Math.PI;
                var nx = cx + r * Math.cos(needleAngle);
                var ny = cy + r * Math.sin(needleAngle);
                ctx.beginPath();
                ctx.arc(nx, ny, lw/2, 0, 2 * Math.PI);
                ctx.fillStyle = arc;
                ctx.fill();
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
