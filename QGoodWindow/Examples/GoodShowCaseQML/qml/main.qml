/*
The MIT License (MIT)

Copyright © 2018-2023 Antonio Dias (https://github.com/antonypro)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

import QtQuick 2.3
import QtQuick.Window 2.2

Rectangle
{
    visible: true
    focus: true

    color: "#303030"

    property string windowIcon
    property string windowIconGrayed
    property string windowTitle
    property bool isMaximized
    property bool isVisible
    property bool isActive
    property bool isTitleBarVisible

    function setButtonState(button, state)
    {
        switch (button)
        {
        case "minBtn":
            switch (state)
            {
            case "hover_enter":
                minBtn.hoverEnter()
                break;
            case "hover_leave":
                minBtn.hoverLeave()
                break;
            case "button_press":
                minBtn.buttonPress()
                break;
            case "button_release":
                minBtn.buttonRelease()
                break;
            default:
                break;
            }
            break;
        case "maxBtn":
            switch (state)
            {
            case "hover_enter":
                maxBtn.hoverEnter()
                break;
            case "hover_leave":
                maxBtn.hoverLeave()
                break;
            case "button_press":
                maxBtn.buttonPress()
                break;
            case "button_release":
                maxBtn.buttonRelease()
                break;
            default:
                break;
            }
            break;
        case "clsBtn":
            switch (state)
            {
            case "hover_enter":
                clsBtn.hoverEnter()
                break;
            case "hover_leave":
                clsBtn.hoverLeave()
                break;
            case "button_press":
                clsBtn.buttonPress()
                break;
            case "button_release":
                clsBtn.buttonRelease()
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    Column
    {
        anchors.fill: parent

        Rectangle
        {
            id: titlebar
            color: "#222222"
            width: parent.width
            height: 30
            z: 1
            visible: isTitleBarVisible

            Rectangle
            {
                id: windowIconRect
                x: 0
                y: 0
                width: 30
                height: parent.height
                color: "transparent"

                Image
                {
                    id: windowIconImage
                    anchors.centerIn: parent
                    width: 16
                    height: 16
                    smooth: true
                    source: isTitleBarVisible ? (isActive ? windowIcon : windowIconGrayed) : ""
                }
            }

            Text
            {
                id: windowTitleText
                x: windowIconRect.width
                y: 0
                anchors.left: windowIconRect.right
                anchors.right: minBtn.left
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Qt.AlignCenter
                color: isActive ? windowTitle.color = "#FFFFFF" : windowTitle.color = "#AAAAAA"
                elide: Text.ElideRight
                text: windowTitle
            }

            CaptionButton
            {
                id: minBtn
                x: parent.width - clsBtn.width - maxBtn.width - width
                y: 0
                width: parent.height
                height: parent.height
                buttonColor: "#009900"
                buttonColorPressed: "#005500"
                imgSource: isTitleBarVisible ? loadPixmapToString(":/icons/minimize.svg") : ""
            }

            CaptionButton
            {
                id: maxBtn
                x: parent.width - clsBtn.width - width
                y: 0
                width: parent.height
                height: parent.height
                buttonColor: "#999900"
                buttonColorPressed: "#555500"
                imgSource: isTitleBarVisible ? (!isMaximized ? loadPixmapToString(":/icons/maximize.svg") :
                                                               loadPixmapToString(":/icons/restore.svg")) : ""
            }

            CaptionButton
            {
                id: clsBtn
                x: parent.width - width
                y: 0
                width: parent.height
                height: parent.height
                buttonColor: "#FF0000"
                buttonColorPressed: "#990000"
                imgSource: isTitleBarVisible ? loadPixmapToString(":/icons/close.svg") : ""
            }
        }

        Rectangle
        {
            id: contents
            color: "transparent"
            y: (titlebar.visible ? titlebar.height : 0)
            width: parent.width
            height: parent.height - (titlebar.visible ? titlebar.height : 0)

            Rectangle
            {
                color: "green"
                width: Math.min(parent.width, parent.height) / 2
                height: Math.min(parent.width, parent.height) / 2
                anchors.centerIn: parent

                RotationAnimation on rotation
                {
                    loops: Animation.Infinite
                    duration: 1000
                    from: 0
                    to: 360
                }
            }
        }
    }
}
