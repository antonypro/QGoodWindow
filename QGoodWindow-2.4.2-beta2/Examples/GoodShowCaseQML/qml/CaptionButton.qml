/*
The MIT License (MIT)

Copyright © 2023 Antonio Dias (https://github.com/antonypro)

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

import QtQuick 2.0

Rectangle
{
    property url imgSource
    property color buttonColor
    property color buttonColorPressed
    property bool buttonPressed: false

    color: "transparent"

    function hoverEnter()
    {
        if (!buttonPressed)
        {
            rect.color = buttonColor
            opacityAnimatorLeave.stop()
            opacityAnimatorEnter.start()
        }
        else
        {
            rect.color = buttonColorPressed
            opacityAnimatorEnter.stop()
            opacityAnimatorLeave.stop()
            rect.opacity = 1.0
        }
    }

    function hoverLeave()
    {
        if (!buttonPressed)
        {
            rect.color = buttonColor
            opacityAnimatorEnter.stop()
            opacityAnimatorLeave.start()
        }
        else
        {
            opacityAnimatorEnter.stop()
            opacityAnimatorLeave.stop()
            rect.opacity = 0.0
        }
    }

    function buttonPress()
    {
        buttonPressed = true

        rect.color = buttonColorPressed
        opacityAnimatorEnter.stop()
        opacityAnimatorLeave.stop()
    }

    function buttonRelease()
    {
        buttonPressed = false

        opacityAnimatorEnter.stop()
        opacityAnimatorLeave.stop()
        rect.opacity = 0.0
    }

    Rectangle
    {
        id: rect
        anchors.centerIn: parent
        width: 20
        height: 20
        radius: 20
        opacity: 0.0

        NumberAnimation on opacity
        {
            id: opacityAnimatorEnter
            to: 1.0
            duration: 100
            running: false
        }

        NumberAnimation on opacity
        {
            id: opacityAnimatorLeave
            to: 0.0
            duration: 100
            running: false
        }
    }

    Image
    {
        id: icon
        anchors.centerIn: rect
        width: 10
        height: 10
        smooth: true
        source: imgSource
    }
}
