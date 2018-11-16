import QtQuick 2.3
import QtQuick.Window 2.2

Rectangle
{
    visible: true
    focus: true

    color: "#303030"

    Rectangle
    {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 100;
        height: 100;
        color: "green"
        RotationAnimation on rotation
        {
            loops: Animation.Infinite
            duration: 1000
            from: 0
            to: 360
        }
    }
}
