/*
The MIT License (MIT)

Copyright © 2018-2021 Antonio Dias

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

#include "winwidget.h"
#include "common.h"
#include "qgoodwindow.h"

WinWidget::WinWidget(QGoodWindow *gw) : QWidget()
{
    HWND parent_hwnd = HWND(gw->winId());
    m_gw = gw;

    setProperty("_q_embedded_native_parent_handle", WId(parent_hwnd));
    setWindowFlags(Qt::FramelessWindowHint);

    QEvent event(QEvent::EmbeddingControl);
    QApplication::sendEvent(this, &event);
}

bool WinWidget::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::ChildPolished:
    {
        QChildEvent *child_event = static_cast<QChildEvent*>(event);

        if (qobject_cast<QDialog*>(child_event->child()))
        {
            //Pass QDialog to main window,
            //to allow move the dialog to the center of the main window.
            return QApplication::sendEvent(m_gw, child_event);
        }

        break;
    }
    default:
        break;
    }

    return QWidget::event(event);
}
