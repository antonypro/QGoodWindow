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

#include "QGoodWindowSource/winwidget.h"
#include "QGoodWindowSource/common.h"

WinWidget::WinWidget(HWND parent_hwnd, QGoodWindow *good_window) : QWinWidget(parent_hwnd)
{
    m_good_window = good_window;
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
        bool maximized = m_good_window->isMaximized();
        bool fullscreen = m_good_window->isFullScreen();

        QRegion region;

        QRect window = rect();

        QRect title_bar = QRect(0, 0, width(), m_title_bar_height);

        QRect left = QRect(m_icon_width, 0, m_left, m_title_bar_height);

        QRect right = QRect(width() - m_right, 0, m_right, m_title_bar_height);

        region = window;

        region -= title_bar;

        region += left;

        region += right;

        if (!maximized && !fullscreen)
        {
            const int adjust_size = 1;

            region -= QRect(0, 0, width(), adjust_size); //Top
            region -= QRect(0, 0, adjust_size, height()); //Left
            region -= QRect(0, height() - 1, width(), adjust_size); //Bottom
            region -= QRect(width() - 1, height() - 1, adjust_size, height()); //Right
        }

        QPoint pos = QPoint(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));

        if (region.contains(mapFromGlobal(pos)))
            break;

        //If region not contains the mouse, pass the
        //WM_NCHITTEST event to main window.

        *result = HTTRANSPARENT;
        return true;
    }
    case WM_SYSKEYDOWN:
    {
        if (msg->wParam == VK_SPACE)
        {
            //Pass ALT+SPACE to main window.
            SendMessageW(parentWindow(), msg->message, msg->wParam, msg->lParam);
            return true;
        }

        break;
    }
    case WM_KEYDOWN:
    {
        if (GetKeyState(VK_SHIFT) & 0x8000)
        {
            switch (msg->wParam)
            {
            case VK_TAB:
                //Prevent that SHIFT+TAB crashes the application.
                return true;
            default:
                break;
            }
        }

        break;
    }
    case WM_CONTEXTMENU:
    {
        //Pass WM_CONTEXTMENU to main window.
        SendMessageW(parentWindow(), msg->message, msg->wParam, msg->lParam);
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
        {
            //Pass QDialog to main window,
            //to allow move the dialog to the center of the main window.
            return QApplication::sendEvent(m_good_window, child_event);
        }

        break;
    }
    default:
        break;
    }

    return QWinWidget::event(event);
}
