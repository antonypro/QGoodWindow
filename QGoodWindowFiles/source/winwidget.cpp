#include "include/winwidget.h"

WinWidget::WinWidget(HWND parent) : QWinWidget(parent)
{
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
            return TRUE;
        }

        break;
    }
    default:
        break;
    }

    return QWinWidget::nativeEvent(eventType, message, result);
}
