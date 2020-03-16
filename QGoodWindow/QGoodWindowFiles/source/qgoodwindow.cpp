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

#include "include/qgoodwindow.h"

#ifdef Q_OS_WIN
#include "include/common.h"
#include "include/winwidget.h"
#include "include/shadow.h"
#define BORDERWIDTH (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER))
static QColor clear_color = QColor(Qt::white);
#endif

QGoodWindow::QGoodWindow(QWidget *parent) : QMainWindow(parent)
{
#ifdef Q_OS_WIN
    m_is_full_screen = false;

    m_last_shadow_state_hidden = true;
    m_last_is_visible = false;
    m_last_minimized = false;
    m_last_maximized = false;
    m_last_fullscreen = false;
    m_last_visible = false;
    m_fixed_size = false;

    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASSEX wcx;
    memset(&wcx, 0, sizeof(WNDCLASSEX));
    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.hInstance = hInstance;
    wcx.lpfnWndProc = WNDPROC(WndProc);
    wcx.cbClsExtra	= 0;
    wcx.cbWndExtra	= 0;
    wcx.hCursor	= LoadCursor(nullptr, IDC_ARROW);
    wcx.hbrBackground = HBRUSH(CreateSolidBrush(RGB(clear_color.red(), clear_color.green(), clear_color.blue())));
    wcx.lpszClassName = TEXT("QGoodWindowClass");

    ATOM windowClass = RegisterClassEx(&wcx);

    m_hwnd = CreateWindowEx(0, reinterpret_cast<WCHAR*>(windowClass), nullptr,
                            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

    if (!m_hwnd)
    {
        QMessageBox::critical(nullptr, "Error", "Error creating window");
        return;
    }

    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    m_helper_widget = new QWidget();

    m_win_widget = new WinWidget(m_hwnd, this);

    m_shadow = new Shadow(m_hwnd);

    m_window_handle = QWindow::fromWinId(WId(m_hwnd));

    QMainWindow::setParent(m_win_widget);
    QMainWindow::setWindowFlags(Qt::Widget);

    QGridLayout *layout = new QGridLayout(m_win_widget);
    layout->setMargin(0);
    layout->addWidget(this);

    QScreen *screen = QApplication::primaryScreen();
    m_pixel_ratio = screen->logicalDotsPerInch() / qreal(96);

    setMargins(30, 0, 0, 0);

    if (!QtWin::isCompositionEnabled())
    {
        disableCaption();
        frameChanged();
    }
#endif
}

QGoodWindow::~QGoodWindow()
{

}

#ifdef Q_OS_WIN
LRESULT QGoodWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    QGoodWindow *window = reinterpret_cast<QGoodWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (!window)
        return DefWindowProc(hwnd, message, wParam, lParam);

    switch (message)
    {
    case WM_ACTIVATE:
    {
        switch (wParam)
        {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
        {
            QTimer::singleShot(0, window, &QGoodWindow::handleActivation);

            break;
        }
        case WA_INACTIVE:
        {
            if (qApp->focusWidget())
                window->m_focus_widget = qApp->focusWidget();

            QTimer::singleShot(0, window, &QGoodWindow::handleDeactivation);

            break;
        }
        default:
            break;
        }

        break;
    }
    case WM_ENABLE:
    {
        if (!window->m_shadow)
            break;

        BOOL enabled = BOOL(wParam);

        if (enabled && !window->isMaximized() && !window->isFullScreen())
            window->m_shadow->show();
        else
            window->m_shadow->hide();

        break;
    }
    case WM_SETFOCUS:
    {
        QTimer::singleShot(0, window, &QGoodWindow::handleActivation);

        break;
    }
    case WM_SIZE:
    {
        window->sizeMoveWindow();

        break;
    }
    case WM_MOVE:
    {
        window->sizeMoveWindow();

        QMoveEvent event = QMoveEvent(QPoint(window->x(), window->y()), QPoint());
        QApplication::sendEvent(window, &event);

        break;
    }
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);

        QSize minimum = window->minimumSize();

        QSize sizeHint = window->minimumSizeHint();

        mmi->ptMinTrackSize.x = qMax(minimum.width(), sizeHint.width());
        mmi->ptMinTrackSize.y = qMax(minimum.height(), sizeHint.height());

        QSize maximum = window->maximumSize();

        mmi->ptMaxTrackSize.x = maximum.width();
        mmi->ptMaxTrackSize.y = maximum.height();

        break;
    }
    case WM_CLOSE:
    {
        QCloseEvent event;
        QApplication::sendEvent(window, &event);

        if (!event.isAccepted())
            return 0;

        window->QMainWindow::setParent(nullptr);

        break;
    }
    case WM_DESTROY:
    {
        delete window->m_win_widget;
        delete window->m_helper_widget;
        delete window->m_shadow;

        if (window->testAttribute(Qt::WA_DeleteOnClose))
            window->deleteLater();

        break;
    }
    case WM_NCHITTEST:
    {
        HRESULT lresult = HRESULT(window->winNCHITTEST(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));

        if (window->m_is_full_screen)
        {
            lresult = HTNOWHERE;

            return lresult;
        }

        if (window->m_fixed_size)
        {
            if (lresult != HTNOWHERE && lresult != HTCAPTION)
                lresult = HTNOWHERE;

            return lresult;
        }

        return lresult;
    }
    case WM_CONTEXTMENU:
    {
        int x = window->x() + (!window->isMaximized() ? BORDERWIDTH : 0);
        int y = window->y() + window->titleBarHeight() - (window->isMaximized() ? BORDERWIDTH : 0);

        window->showContextMenu(x, y);

        break;
    }
    case WM_NCRBUTTONUP:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        LRESULT lRet = window->winNCHITTEST(x, y);

        if (lRet == HTCAPTION || lRet == HTSYSMENU)
            window->showContextMenu(x, y);

        break;
    }
    case WM_NCCALCSIZE:
    {
        if (window->isFullScreen())
            break;

        if (wParam == TRUE && window->isMaximized())
            InflateRect(reinterpret_cast<RECT*>(lParam), -BORDERWIDTH, -BORDERWIDTH);

        return TRUE;
    }
    case WM_NCPAINT:
    case WM_NCACTIVATE:
    {
        if (!QtWin::isCompositionEnabled())
            return TRUE;

        break;
    }
    case WM_WINDOWPOSCHANGED:
    {
        if (window->isVisible() != window->m_last_is_visible
                || window->isMinimized() != window->m_last_minimized
                || window->isMaximized() != window->m_last_maximized
                || window->isFullScreen() != window->m_last_fullscreen)
        {
            QWindowStateChangeEvent event = QWindowStateChangeEvent(Qt::WindowNoState);
            QApplication::sendEvent(window, &event);

            window->m_last_is_visible = window->isVisible();
            window->m_last_minimized = window->isMinimized();
            window->m_last_maximized = window->isMaximized();
            window->m_last_fullscreen = window->isFullScreen();
        }

        if (!QtWin::isCompositionEnabled() && window->isMaximized())
        {
            window->enableCaption();
            QTimer::singleShot(0, window, &QGoodWindow::disableCaption);
        }

        if (window->isMinimized())
        {
            if (qApp->focusWidget())
                window->m_focus_widget = qApp->focusWidget();
        }

        WINDOWPOS *pwp = reinterpret_cast<WINDOWPOS*>(lParam);

        if (pwp->flags & SWP_SHOWWINDOW)
        {
            QShowEvent event = QShowEvent();
            QApplication::sendEvent(window, &event);

            if (window->m_fixed_size)
            {
                SetWindowLongPtr(hwnd, GWL_STYLE, GetWindowLongPtr(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
            }

            //SET NULL BRUSH
            {
                HBRUSH brush = HBRUSH(GetStockObject(NULL_BRUSH));
                HBRUSH oldbrush = HBRUSH(SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, LONG_PTR(brush)));

                DeleteObject(oldbrush);
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            window->m_shadow->showLater();
            window->m_last_visible = true;

            window->m_win_widget->show();

            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        }
        else if (pwp->flags & SWP_HIDEWINDOW)
        {
            if (qApp->focusWidget())
                window->m_focus_widget = qApp->focusWidget();

            QHideEvent event = QHideEvent();
            QApplication::sendEvent(window, &event);

            window->m_last_visible = false;
            window->m_shadow->hide();

            //SET BLACK BRUSH
            {
                HBRUSH brush = HBRUSH(GetStockObject(WHITE_BRUSH));
                HBRUSH oldbrush = HBRUSH(SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, LONG_PTR(brush)));

                DeleteObject(oldbrush);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
        }

        break;
    }
    case WM_WINDOWPOSCHANGING:
    {
        bool minimized = window->isMinimized();
        bool maximized = window->isMaximized();

        bool m_last_shadow_state_minimized_maximized = false;

        if (window->m_last_shadow_state_hidden && !minimized && !maximized)
        {
            m_last_shadow_state_minimized_maximized = true;
        }

        if (maximized || minimized)
        {
            window->m_last_shadow_state_hidden = true;
            if (window->m_last_visible)
            {
                window->m_shadow->hide();
            }
        }
        else if (m_last_shadow_state_minimized_maximized)
        {
            if (window->m_last_visible)
            {
                window->m_shadow->showLater();
            }
            window->m_last_shadow_state_hidden = false;
        }

        break;
    }
    case WM_SETTEXT:
    {
        emit window->windowTitleChanged(window->windowTitle());
        break;
    }
    case WM_SETICON:
    {
        emit window->windowIconChanged(window->windowIcon());
        break;
    }
    case WM_SETTINGCHANGE:
    case WM_THEMECHANGED:
    case WM_DWMCOMPOSITIONCHANGED:
    {
        SendMessage(HWND(window->m_helper_widget->winId()), message, 0, 0);

        if (QtWin::isCompositionEnabled())
        {
            QTimer::singleShot(500, window, &QGoodWindow::enableCaption);
            QTimer::singleShot(800, window, &QGoodWindow::frameChanged);

            QTimer::singleShot(1000, window, [=]{
                SetWindowRgn(hwnd, nullptr, TRUE);
            });
        }
        else
        {
            QTimer::singleShot(500, window, &QGoodWindow::disableCaption);
            QTimer::singleShot(800, window, &QGoodWindow::frameChanged);
        }

        QTimer::singleShot(500, window, &QGoodWindow::sizeMoveWindow);
        QTimer::singleShot(500, window, static_cast<void(QGoodWindow::*)(void)>(&QGoodWindow::repaint));

        break;
    }
    default:
        break;
    }

    MSG msg;
    msg.hwnd = hwnd;
    msg.message = message;
    msg.lParam = lParam;
    msg.wParam = wParam;

    long result;

    bool ret = window->nativeEvent(QByteArray(), &msg, &result);

    if (ret)
        return result;

    return DefWindowProc(hwnd, message, wParam, lParam);
}
#endif

#ifdef Q_OS_WIN
void QGoodWindow::handleActivation()
{
    if (!m_win_widget || !m_shadow)
        return;

    if (!m_focus_widget)
        m_focus_widget = qApp->focusWidget();

    SetFocus(HWND(m_win_widget->winId()));

    m_shadow->setActive(true);

    if (m_focus_widget)
        m_focus_widget->setFocus();

    if (!isMaximized() && !isFullScreen())
        m_shadow->show();
}
#endif

#ifdef Q_OS_WIN
void QGoodWindow::handleDeactivation()
{
    if (!m_win_widget || !m_shadow)
        return;

    m_shadow->setActive(false);

    if (!isMaximized() && !isFullScreen())
        m_shadow->show();
}
#endif

#ifdef Q_OS_WIN
void QGoodWindow::enableCaption()
{
    SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowLongPtr(m_hwnd, GWL_STYLE) | WS_CAPTION);
}
#endif

#ifdef Q_OS_WIN
void QGoodWindow::disableCaption()
{
    SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowLongPtr(m_hwnd, GWL_STYLE) & ~WS_CAPTION);
}
#endif

#ifdef Q_OS_WIN
void QGoodWindow::frameChanged()
{
    SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}
#endif

void QGoodWindow::setFixedSize(int w, int h)
{
#ifdef Q_OS_WIN
    m_fixed_size = true;
    setMinimumSize(w, h);
    setMaximumSize(w, h);
    resize(w, h);
#else
    QMainWindow::setFixedSize(w, h);
#endif
}

void QGoodWindow::setFixedSize(const QSize &size)
{
#ifdef Q_OS_WIN
    setFixedSize(size.width(), size.height());
#else
    QMainWindow::setFixedSize(size);
#endif
}

QRect QGoodWindow::frameGeometry() const
{
#ifdef Q_OS_WIN
    RECT window_rect;
    GetWindowRect(m_hwnd, &window_rect);

    QRect rect =  QRect(window_rect.left, window_rect.top,
                        window_rect.right - window_rect.left,
                        window_rect.bottom - window_rect.top);

    return rect;
#else
    return QMainWindow::frameGeometry();
#endif
}

QRect QGoodWindow::geometry() const
{
#ifdef Q_OS_WIN
    return frameGeometry();
#else
    return QMainWindow::geometry();
#endif
}

QRect QGoodWindow::rect() const
{
#ifdef Q_OS_WIN
    return QRect(0, 0, width(), height());
#else
    return QMainWindow::rect();
#endif
}

QPoint QGoodWindow::pos() const
{
#ifdef Q_OS_WIN
    QPoint p = frameGeometry().topLeft();

    if (isMaximized())
    {
        p.setX(p.x() + BORDERWIDTH);
        p.setY(p.y() + BORDERWIDTH);
    }

    return p;
#else
    return QMainWindow::pos();
#endif
}

int QGoodWindow::x() const
{
#ifdef Q_OS_WIN
    return pos().x();
#else
    return QMainWindow::x();
#endif
}

int QGoodWindow::y() const
{
#ifdef Q_OS_WIN
    return pos().y();
#else
    return QMainWindow::y();
#endif
}

int QGoodWindow::width() const
{
#ifdef Q_OS_WIN
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    return (rect.right - rect.left);
#else
    return QMainWindow::width();
#endif
}

int QGoodWindow::height() const
{
#ifdef Q_OS_WIN
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    return (rect.bottom - rect.top);
#else
    return QMainWindow::height();
#endif
}

void QGoodWindow::move(int x, int y)
{
#ifdef Q_OS_WIN
    SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
#else
    QMainWindow::move(x, y);
#endif
}

void QGoodWindow::move(const QPoint &pos)
{
#ifdef Q_OS_WIN
    move(pos.x(), pos.y());
#else
    QMainWindow::move(pos);
#endif
}

void QGoodWindow::resize(int width, int height)
{
#ifdef Q_OS_WIN
    SetWindowPos(m_hwnd, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOACTIVATE);
#else
    QMainWindow::resize(width, height);
#endif
}

void QGoodWindow::resize(const QSize &size)
{
#ifdef Q_OS_WIN
    resize(size.width(), size.height());
#else
    QMainWindow::resize(size);
#endif
}

void QGoodWindow::setGeometry(int x, int y, int w, int h)
{
#ifdef Q_OS_WIN
    move(x, y);
    resize(w, h);
#else
    QMainWindow::setGeometry(x, y, w, h);
#endif
}

void QGoodWindow::setGeometry(const QRect &rect)
{
#ifdef Q_OS_WIN
    setGeometry(rect.x(), rect.y(), rect.width(), rect.height());
#else
    QMainWindow::setGeometry(rect);
#endif
}

int QGoodWindow::titleBarHeight() const
{
#ifdef Q_OS_WIN
    return m_title_bar_height;
#else
    return 0;
#endif
}

int QGoodWindow::iconWidth() const
{
#ifdef Q_OS_WIN
    return m_icon_width;
#else
    return 0;
#endif
}

int QGoodWindow::leftMargin() const
{
#ifdef Q_OS_WIN
    return m_left_margin;
#else
    return 0;
#endif
}

int QGoodWindow::rightMargin() const
{
#ifdef Q_OS_WIN
    return m_right_margin;
#else
    return 0;
#endif
}

void QGoodWindow::setMargins(int title_bar_height, int icon_width, int left, int right)
{
#ifdef Q_OS_WIN
    if (!m_win_widget)
        return;

    if (title_bar_height < 0)
        title_bar_height = 0;

    m_title_bar_height = title_bar_height;

    if (icon_width < 0)
        icon_width = 0;

    m_icon_width = icon_width;

    if (left < 0)
        left = 0;

    m_left_margin = left;

    if (right < 0)
        right = 0;

    m_right_margin = right;

    m_win_widget->setMargins(title_bar_height, icon_width, left, right);
#else
    Q_UNUSED(title_bar_height)
    Q_UNUSED(icon_width)
    Q_UNUSED(left)
    Q_UNUSED(right)
#endif
}

void QGoodWindow::activateWindow()
{
#ifdef Q_OS_WIN
    SetForegroundWindow(m_hwnd);
#else
    QMainWindow::activateWindow();
#endif
}

void QGoodWindow::show()
{
#ifdef Q_OS_WIN
    ShowWindow(m_hwnd, SW_SHOW);
#else
    QMainWindow::show();
#endif
}

void QGoodWindow::showNormal()
{
#ifdef Q_OS_WIN
    ShowWindow(m_hwnd, SW_SHOWNORMAL);

    if (m_is_full_screen)
    {
        m_is_full_screen = false;
        SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowLongPtr(m_hwnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);
        setGeometry(m_rect_origin);
        m_rect_origin = QRect();
        sizeMoveWindow();

        m_shadow->show();

        QWindowStateChangeEvent event = QWindowStateChangeEvent(Qt::WindowNoState);
        QApplication::sendEvent(this, &event);
    }
#else
    QMainWindow::showNormal();
#endif
}

void QGoodWindow::showMaximized()
{
#ifdef Q_OS_WIN
    if (m_fixed_size)
        return;

    if (!isVisible())
    {
        disableCaption();
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        enableCaption();
    }

    ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
#else
    QMainWindow::showMaximized();
#endif
}

void QGoodWindow::showMinimized()
{
#ifdef Q_OS_WIN
    ShowWindow(m_hwnd, SW_SHOWMINIMIZED);
#else
    QMainWindow::showMinimized();
#endif
}

void QGoodWindow::showFullScreen()
{
#ifdef Q_OS_WIN
    if (m_fixed_size)
        return;

    ShowWindow(m_hwnd, SW_SHOWNORMAL);

    m_rect_origin = frameGeometry();

    m_is_full_screen = true;
    SetWindowLongPtr(m_hwnd, GWL_STYLE, GetWindowLongPtr(m_hwnd, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
    sizeMoveWindow();

    QWindowStateChangeEvent event = QWindowStateChangeEvent(Qt::WindowNoState);
    QApplication::sendEvent(this, &event);
#else
    QMainWindow::showFullScreen();
#endif
}

void QGoodWindow::hide()
{
#ifdef Q_OS_WIN
    ShowWindow(m_hwnd, SW_HIDE);
#else
    QMainWindow::hide();
#endif
}

void QGoodWindow::close()
{
#ifdef Q_OS_WIN
    PostMessage(m_hwnd, WM_CLOSE, 0, 0);
#else
    QMainWindow::close();
#endif
}

bool QGoodWindow::isVisible() const
{
#ifdef Q_OS_WIN
    return IsWindowVisible(m_hwnd);
#else
    return QMainWindow::isVisible();
#endif
}

bool QGoodWindow::isEnabled() const
{
#ifdef Q_OS_WIN
    return IsWindowEnabled(m_hwnd);
#else
    return QMainWindow::isEnabled();
#endif
}

bool QGoodWindow::isActiveWindow() const
{
#ifdef Q_OS_WIN
    return (GetForegroundWindow() == m_hwnd);
#else
    return QMainWindow::isActiveWindow();
#endif
}

bool QGoodWindow::isMaximized() const
{
#ifdef Q_OS_WIN
    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(m_hwnd, &wp);
    return (wp.showCmd == SW_SHOWMAXIMIZED);
#else
    return QMainWindow::isMaximized();
#endif
}

bool QGoodWindow::isMinimized() const
{
#ifdef Q_OS_WIN
    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(m_hwnd, &wp);
    return (wp.showCmd == SW_SHOWMINIMIZED);
#else
    return QMainWindow::isMinimized();
#endif
}

bool QGoodWindow::isFullScreen() const
{
#ifdef Q_OS_WIN
    return m_is_full_screen;
#else
    return QMainWindow::isFullScreen();
#endif
}

#ifdef Q_OS_WIN
void QGoodWindow::sizeMoveWindow()
{
    if (!m_win_widget || !m_shadow)
        return;

    if (isFullScreen())
    {
        QScreen *screen = QApplication::primaryScreen();
        setGeometry(screen->geometry());
    }

    if (!QtWin::isCompositionEnabled())
    {
        QRect rect = QRect(0, 0, frameGeometry().width(), frameGeometry().height());

        if (isMaximized() && !isFullScreen())
            rect.adjust(BORDERWIDTH, BORDERWIDTH, -BORDERWIDTH, -BORDERWIDTH);

        SetWindowRgn(m_hwnd, QtWin::toHRGN(rect), TRUE);
    }

    RECT client_rect;
    GetClientRect(m_hwnd, &client_rect);

    if (!isMaximized() && !isFullScreen())
    {
        m_win_widget->move(0, 0);
        m_win_widget->resize(client_rect.right - client_rect.left, client_rect.bottom - client_rect.top);

        m_shadow->move(frameGeometry().x() - SHADOWWIDTH, frameGeometry().y() - SHADOWWIDTH);
        m_shadow->resize(frameGeometry().width() + SHADOWWIDTH * 2, frameGeometry().height() + SHADOWWIDTH * 2);
    }
    else
    {
        m_win_widget->move(0, 0);
        m_win_widget->resize(client_rect.right - client_rect.left, client_rect.bottom - client_rect.top);

        m_shadow->hide();
    }
}
#endif

QWindow *QGoodWindow::windowHandle() const
{
#ifdef Q_OS_WIN
    return m_window_handle;
#else
    return QMainWindow::windowHandle();
#endif
}

qreal QGoodWindow::windowOpacity() const
{
    return windowHandle()->opacity();
}

void QGoodWindow::setWindowOpacity(qreal level)
{
    windowHandle()->setOpacity(level);
}

void QGoodWindow::setWindowTitle(const QString &title)
{
#ifdef Q_OS_WIN
    if (!m_hwnd)
        return;

    SetWindowText(m_hwnd, reinterpret_cast<WCHAR*>(const_cast<QChar*>(title.unicode())));

    QWidget::setWindowTitle(title);
#else
    return QMainWindow::setWindowTitle(title);
#endif
}

void QGoodWindow::setWindowIcon(const QPixmap &icon)
{
#ifdef Q_OS_WIN
    if (!m_hwnd)
        return;

    HICON hicon_big = QtWin::toHICON(icon.scaled(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON),
                                                 Qt::KeepAspectRatio, Qt::SmoothTransformation));

    HICON hicon_small = QtWin::toHICON(icon.scaled(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));

    SendMessage(m_hwnd, WM_SETICON, ICON_BIG, LPARAM(hicon_big));
    SendMessage(m_hwnd, WM_SETICON, ICON_SMALL, LPARAM(hicon_small));

    QWidget::setWindowIcon(QIcon(icon));
#else
    return QMainWindow::setWindowIcon(icon);
#endif
}

#ifdef Q_OS_WIN
LRESULT QGoodWindow::winNCHITTEST(int x, int y)
{
    LONG borderWidth = 1; //in pixels
    LONG titleHeight = titleBarHeight(); //in pixels

    // Get the point coordinates for the hit test.
    POINT ptMouse = {x, y};

    // Get the window rectangle.
    RECT rcWindow;
    GetWindowRect(m_hwnd, &rcWindow);

    // Determine if the hit test is for resizing. Default middle (1,1).
    USHORT uRow = 1;
    USHORT uCol = 1;
    bool fOnResizeBorder = false;

    WINDOWPLACEMENT wp;
    GetWindowPlacement(m_hwnd, &wp);
    bool maximized = (wp.showCmd == SW_SHOWMAXIMIZED);

    if (maximized)
        titleHeight += GetSystemMetrics(SM_CYSIZEFRAME);

    // Determine if the point is at the top or bottom of the window.
    if (ptMouse.y < rcWindow.top + titleHeight)
    {
        fOnResizeBorder = (ptMouse.y < (rcWindow.top + borderWidth));
        uRow = 0;
    }
    else if (ptMouse.y >= rcWindow.bottom - borderWidth)
    {
        uRow = 2;
    }

    // Determine if the point is at the left or right of the window.
    if (ptMouse.x < rcWindow.left + borderWidth)
    {
        uCol = 0; // left side
    }
    else if (ptMouse.x >= rcWindow.right - borderWidth)
    {
        uCol = 2; // right side
    }
    else if (uRow == 0 && !fOnResizeBorder)
    {
        if (!maximized)
        {
            if (ptMouse.x >= rcWindow.right - rightMargin())
                return HTNOWHERE; // title bar buttons
            else if (ptMouse.x > rcWindow.left + iconWidth() && ptMouse.x < rcWindow.left + iconWidth() + leftMargin())
                return HTNOWHERE; // custom title bar buttons
            else if (ptMouse.x < rcWindow.left + iconWidth())
                return HTSYSMENU; // title bar icon
        }
        else
        {
            if (ptMouse.x >= rcWindow.right - BORDERWIDTH - rightMargin())
                return HTNOWHERE; // maximized title bar buttons
            else if (ptMouse.x > rcWindow.left + BORDERWIDTH + iconWidth() && ptMouse.x < rcWindow.left + BORDERWIDTH + iconWidth() + leftMargin())
                return HTNOWHERE; // custom title bar buttons
            else if (ptMouse.x < rcWindow.left + BORDERWIDTH + iconWidth())
                return HTSYSMENU; // maximized title bar icon
        }
    }

    // Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
    LRESULT hitTests[3][3] =
    {
        {HTTOPLEFT, fOnResizeBorder ? HTTOP : HTCAPTION, HTTOPRIGHT},
        {HTLEFT, HTNOWHERE, HTRIGHT},
        {HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT},
    };

    return hitTests[uRow][uCol];
}
#endif

#ifdef Q_OS_WIN
void QGoodWindow::showContextMenu(int x, int y)
{
    HMENU hMenu = GetSystemMenu(m_hwnd, FALSE);

    if (!hMenu)
        return;

    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STATE;
    mii.fType = 0;

    mii.fState = MF_ENABLED;
    SetMenuItemInfo(hMenu, SC_RESTORE, FALSE, &mii);
    SetMenuItemInfo(hMenu, SC_SIZE, FALSE, &mii);
    SetMenuItemInfo(hMenu, SC_MOVE, FALSE, &mii);
    SetMenuItemInfo(hMenu, SC_MAXIMIZE, FALSE, &mii);
    SetMenuItemInfo(hMenu, SC_MINIMIZE, FALSE, &mii);

    mii.fState = MF_GRAYED;

    WINDOWPLACEMENT wp;
    GetWindowPlacement(m_hwnd, &wp);

    switch (wp.showCmd)
    {
    case SW_SHOWMAXIMIZED:
        SetMenuItemInfo(hMenu, SC_SIZE, FALSE, &mii);
        SetMenuItemInfo(hMenu, SC_MOVE, FALSE, &mii);
        SetMenuItemInfo(hMenu, SC_MAXIMIZE, FALSE, &mii);
        SetMenuDefaultItem(hMenu, SC_CLOSE, FALSE);
        break;
    case SW_SHOWMINIMIZED:
        SetMenuItemInfo(hMenu, SC_MINIMIZE, FALSE, &mii);
        SetMenuDefaultItem(hMenu, SC_RESTORE, FALSE);
        break;
    case SW_SHOWNORMAL:
        SetMenuItemInfo(hMenu, SC_RESTORE, FALSE, &mii);
        SetMenuDefaultItem(hMenu, SC_CLOSE, FALSE);
        break;
    }

    if (!(GetWindowLongPtr(m_hwnd, GWL_STYLE) & WS_MAXIMIZEBOX))
    {
        SetMenuItemInfo(hMenu, SC_MAXIMIZE, FALSE, &mii);
        SetMenuItemInfo(hMenu, SC_RESTORE, FALSE, &mii);
    }

    if (!(GetWindowLongPtr(m_hwnd, GWL_STYLE) & WS_MINIMIZEBOX))
    {
        SetMenuItemInfo(hMenu, SC_MINIMIZE, FALSE, &mii);
    }

    LPARAM cmd = TrackPopupMenu(hMenu, (TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD), x, y, 0, m_hwnd, nullptr);

    if (cmd)
        PostMessage(m_hwnd, WM_SYSCOMMAND, WPARAM(cmd), 0);
}
#endif

WId QGoodWindow::winId() const
{
#ifdef Q_OS_WIN
    return WId(m_hwnd);
#else
    return QMainWindow::winId();
#endif
}

qreal QGoodWindow::pixelRatio() const
{
#ifdef Q_OS_WIN
    return m_pixel_ratio;
#else
    return qreal(1);
#endif
}

void QGoodWindow::setClearColor(const QColor &color)
{
#ifdef Q_OS_WIN
    clear_color = color;
#else
    Q_UNUSED(color)
#endif
}

#ifdef Q_OS_WIN
bool QGoodWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::ChildPolished:
    {
        QChildEvent *child_event = static_cast<QChildEvent*>(event);

        if (QDialog *dialog = qobject_cast<QDialog*>(child_event->child()))
            dialog->installEventFilter(this);

        break;
    }
    case QEvent::WindowBlocked:
    {
        EnableWindow(m_hwnd, FALSE);

        break;
    }
    case QEvent::WindowUnblocked:
    {
        EnableWindow(m_hwnd, TRUE);

        break;
    }
    default:
        break;
    }

    return QMainWindow::event(event);
}
#endif

#ifdef Q_OS_WIN
bool QGoodWindow::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Show:
    {
        if (QDialog *dialog = qobject_cast<QDialog*>(watched))
            sizeDialog(dialog, this);

        break;
    }
    default:
        break;
    }

    return QMainWindow::eventFilter(watched, event);
}
#endif
