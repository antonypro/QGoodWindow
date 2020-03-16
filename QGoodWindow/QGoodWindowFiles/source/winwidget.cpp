/*
The MIT License (MIT)

Copyright © 2018-2020 Antonio Dias

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

#include "include/winwidget.h"
#include "include/common.h"

WinWidget::WinWidget(HWND parent_hwnd, QWidget *parent) : QWinWidget(parent_hwnd)
{
    m_parent = parent;
    m_title_bar_height = 0;
    m_icon_width = 0;
    m_left = 0;
    m_right = 0;

    setMouseTracking(true);
}

void WinWidget::setMargins(int title_bar_height, int icon_width, int left, int right)
{
    m_title_bar_height = title_bar_height;
    m_icon_width = icon_width;
    m_left = left;
    m_right = right;
}

bool WinWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);

    switch (msg->message)
    {
    case WM_NCHITTEST:
    {
        QRegion region;

        int adjust_size = 0;

        WINDOWPLACEMENT wp;
        wp.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(parentWindow(), &wp);
        bool maximized = (wp.showCmd == SW_SHOWMAXIMIZED);

        if (!maximized)
            adjust_size = 1;

        QRect window = rect().adjusted(adjust_size, adjust_size, -adjust_size, -adjust_size);

        QRect title_bar = QRect(0, 0, width() - m_right, m_title_bar_height);

        QRect left = QRect(m_icon_width, 0, m_left, m_title_bar_height);

        region = window;

        region -= title_bar;

        region += left;

        QPoint pos = QPoint(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));

        if (region.contains(mapFromGlobal(pos)))
            break;

        *result = HTTRANSPARENT;
        return true;
    }
    case WM_SYSKEYDOWN:
    {
        if (msg->wParam == VK_SPACE)
        {
            SendMessage(parentWindow(), msg->message, msg->wParam, msg->lParam);
            return true;
        }

        break;
    }
    case WM_KEYDOWN:
    {
        if (GetKeyState(VK_SHIFT) & 0x8000)
        {
            switch(msg->wParam)
            {
            case VK_TAB:
                return true;
            default:
                break;
            }
        }

        break;
    }
    case WM_CONTEXTMENU:
    {
        SendMessage(parentWindow(), msg->message, msg->wParam, msg->lParam);
        return true;
    }
    default:
        break;
    }

    return QWinWidget::nativeEvent(eventType, message, result);
}

bool WinWidget::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::ChildPolished:
    {
        QChildEvent *child_event = static_cast<QChildEvent*>(event);

        if (qobject_cast<QDialog*>(child_event->child()))
            return QApplication::sendEvent(m_parent, child_event);

        break;
    }
    case QEvent::ChildAdded:
    case QEvent::ChildRemoved:
    {
        QChildEvent *child_event = static_cast<QChildEvent*>(event);

        if (child_event->child()->isWidgetType())
            return QApplication::sendEvent(m_parent, child_event);

        break;
    }
    default:
        break;
    }

    return QWinWidget::event(event);
}
