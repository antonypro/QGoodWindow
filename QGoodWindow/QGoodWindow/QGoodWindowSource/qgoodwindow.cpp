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

#include "common.h"
#include "qgoodwindow.h"
#include "shadow.h"

#ifndef QGOODWINDOW
#ifdef Q_OS_WIN
#undef Q_OS_WIN
#endif
#ifdef Q_OS_LINUX
#undef Q_OS_LINUX
#endif
#ifdef Q_OS_MAC
#undef Q_OS_MAC
#endif
#endif

#ifdef Q_OS_WIN

#include <QtWinExtras>
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include "winwidget.h"

class NativeEventFilter : public QAbstractNativeEventFilter
{
public:
    NativeEventFilter(QGoodWindow *gw)
    {
        m_gw = gw;
    }

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override
    {
        Q_UNUSED(eventType)

        HWND m_gw_hwnd = HWND(m_gw->winId());

        MSG *msg = static_cast<MSG*>(message);

        if (!IsWindowVisible(msg->hwnd))
            return false;

        if (!IsChild(m_gw_hwnd, msg->hwnd))
            return false;

        switch (msg->message)
        {
        case WM_NCHITTEST:
        {
            HRESULT lresult = m_gw->ncHitTest(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));

            if (lresult == HTNOWHERE)
                break;

            //If region not contains the mouse, pass the
            //WM_NCHITTEST event to main window.

            *result = HTTRANSPARENT;
            return true;
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
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            //Pass to main window.

            if ((GetKeyState(VK_SHIFT) & 0x8000) && msg->wParam == VK_F10)
            {
                //When SHIFT+F10 is pressed.
                SendMessageW(m_gw_hwnd, msg->message, msg->wParam, msg->lParam);
                return true;
            }

            if ((GetKeyState(VK_MENU) & 0x8000) && msg->wParam == VK_SPACE)
            {
                //When ALT+SPACE is pressed.
                SendMessageW(m_gw_hwnd, msg->message, msg->wParam, msg->lParam);
                return true;
            }

            break;
        }
        default:
            break;
        }

        return false;
    }

private:
    QGoodWindow *m_gw;
};
#endif

#ifdef Q_OS_LINUX

#include <QtX11Extras/QX11Info>
#include <QtTest/QtTest>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <xcb/xcb.h>

#define SHADOW_WINDOW_COUNT 8

#endif

#ifdef Q_OS_MAC

#include "macosnative.h"

Notification notification;

#endif

QGoodWindow::QGoodWindow(QWidget *parent, const QColor &clear_color) : QMainWindow(parent)
{
#ifdef QGOODWINDOW
    setParent(nullptr);
#endif
#ifdef Q_OS_WIN
    m_clear_color = clear_color;
#else
    Q_UNUSED(clear_color)
#endif
#if defined Q_OS_WIN || defined Q_OS_LINUX
    m_fixed_size = false;
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
    m_mouse_button_pressed = false;
#endif
#ifdef Q_OS_WIN
    m_is_full_screen = false;

    m_active_state = false;

    m_state = Qt::WindowNoState;

    m_native_event = nullptr;

    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wcx;
    memset(&wcx, 0, sizeof(WNDCLASSEXW));
    wcx.cbSize = sizeof(WNDCLASSEXW);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.hInstance = hInstance;
    wcx.lpfnWndProc = WNDPROC(WndProc);
    wcx.cbClsExtra	= 0;
    wcx.cbWndExtra	= 0;
    wcx.hCursor	= LoadCursorW(nullptr, IDC_ARROW);
    wcx.hbrBackground = HBRUSH(CreateSolidBrush(RGB(m_clear_color.red(),
                                                    m_clear_color.green(),
                                                    m_clear_color.blue())));
    wcx.lpszClassName = L"QGoodWindowClass";

    RegisterClassExW(&wcx);

    m_hwnd = CreateWindowW(wcx.lpszClassName, nullptr,
                           WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

    if (!m_hwnd)
    {
        QMessageBox::critical(nullptr, "Error", "Error creating window");
        return;
    }

    SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    connect(qApp, &QApplication::aboutToQuit, this, [=]{
        //Fix QApplication::exit() crash.
        SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
    });

    m_window_handle = QWindow::fromWinId(WId(m_hwnd));

    m_helper_widget = new QWidget();

    QScreen *screen = windowHandle()->screen();
    qreal pixel_ratio = screen->logicalDotsPerInch() / qreal(96);

    m_win_widget = new WinWidget(this);

    m_shadow = new Shadow(pixel_ratio, m_hwnd);

    m_main_window = static_cast<QMainWindow*>(this);

    m_main_window->setParent(m_win_widget);
    m_main_window->setWindowFlags(Qt::Widget);

    if (!m_native_event)
    {
        m_native_event = new NativeEventFilter(this);
        qApp->installNativeEventFilter(m_native_event);
    }

    QGridLayout *layout = new QGridLayout(m_win_widget);
    layout->setMargin(0);
    layout->addWidget(this);

    setMargins(30, 0, 0, 0);

    if (!QtWin::isCompositionEnabled())
    {
        disableCaption();
        frameChanged();
    }
#endif
#ifdef Q_OS_LINUX
    m_resize_move = false;
    m_resize_move_started = false;

    for (int i = 0; i < SHADOW_WINDOW_COUNT; i++)
    {
        Shadow *shadow = new Shadow(this);
        shadow->setAttribute(Qt::WA_DeleteOnClose);
        shadow->show();

        shadow->installEventFilter(this);
        shadow->setMouseTracking(true);

        m_shadow_list.append(shadow);
    }

    installEventFilter(this);
    setMouseTracking(true);

    setWindowFlags(Qt::FramelessWindowHint);

    QScreen *screen = windowHandle()->screen();
    m_pixel_ratio = screen->logicalDotsPerInch() / qreal(96);

    setMargins(30, 0, 0, 0);
#endif
#ifdef Q_OS_MAC
    installEventFilter(this);
    setMouseTracking(true);

    notification.addWindow(this);

    notification.registerNotification("NSWindowWillEnterFullScreenNotification", winId());
    notification.registerNotification("NSWindowDidExitFullScreenNotification", winId());

    macOSNative::setStyle(long(winId()), isFullScreen());

    setMargins(30, 0, 0, 0);
#endif
}

QGoodWindow::~QGoodWindow()
{
#ifdef Q_OS_WIN
    if (m_native_event)
    {
        qApp->removeNativeEventFilter(m_native_event);
        delete m_native_event;
        m_native_event = nullptr;
    }
#endif
#ifdef Q_OS_MAC
    notification.removeWindow(this);
#endif
}

WId QGoodWindow::winId() const
{
#ifdef Q_OS_WIN
    return WId(m_hwnd);
#else
    return QMainWindow::winId();
#endif
}

/*** QGoodWindow functions begin ***/

int QGoodWindow::titleBarHeight() const
{
#ifdef QGOODWINDOW
    return m_title_bar_height;
#else
    return 0;
#endif
}

int QGoodWindow::iconWidth() const
{
#ifdef QGOODWINDOW
    return m_icon_width;
#else
    return 0;
#endif
}

int QGoodWindow::leftMargin() const
{
#ifdef QGOODWINDOW
    return m_left_margin;
#else
    return 0;
#endif
}

int QGoodWindow::rightMargin() const
{
#ifdef QGOODWINDOW
    return m_right_margin;
#else
    return 0;
#endif
}

void QGoodWindow::setMargins(int title_bar_height, int icon_width, int left, int right)
{
#ifdef QGOODWINDOW

#ifdef Q_OS_WIN
    if (!m_win_widget)
        return;
#endif

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

    m_left_mask = QRegion(0, 0, m_left_margin, m_title_bar_height);
    m_right_mask = QRegion(0, 0, m_right_margin, m_title_bar_height);

#else
    Q_UNUSED(title_bar_height)
    Q_UNUSED(icon_width)
    Q_UNUSED(left)
    Q_UNUSED(right)
#endif
}

void QGoodWindow::setLeftMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    m_left_mask = mask;
#else
    Q_UNUSED(mask)
#endif
}

void QGoodWindow::setRightMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    m_right_mask = mask;
#else
    Q_UNUSED(mask)
#endif
}

QSize QGoodWindow::leftMaskSize() const
{
#ifdef QGOODWINDOW
    return QSize(m_left_margin, m_title_bar_height);
#else
    return QSize();
#endif
}

QSize QGoodWindow::rightMaskSize() const
{
#ifdef QGOODWINDOW
    return QSize(m_right_margin, m_title_bar_height);
#else
    return QSize();
#endif
}

/*** QGoodWindow functions end ***/

void QGoodWindow::setFixedSize(int w, int h)
{
#if defined Q_OS_WIN || defined Q_OS_LINUX
    m_fixed_size = true;
#endif
#ifdef Q_OS_WIN
    SetWindowLongW(m_hwnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
    resize(w, h);
#else
    QMainWindow::setFixedSize(w, h);
#endif
}

void QGoodWindow::setFixedSize(const QSize &size)
{
#if defined Q_OS_WIN || defined Q_OS_LINUX
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

    if (isMaximized())
    {
        const int border_width = BORDERWIDTH;
        rect.adjust(border_width, border_width, -border_width, -border_width);
    }

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
    return frameGeometry().topLeft();
#else
    return QMainWindow::pos();
#endif
}

QSize QGoodWindow::size() const
{
#ifdef Q_OS_WIN
    return QSize(width(), height());
#else
    return QMainWindow::size();
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
    return frameGeometry().width();
#else
    return QMainWindow::width();
#endif
}

int QGoodWindow::height() const
{
#ifdef Q_OS_WIN
    return frameGeometry().height();
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
    if (m_is_full_screen)
        showNormal();

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
        SetWindowLongW(m_hwnd, GWL_STYLE, GetWindowLongW(m_hwnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);
        setGeometry(m_rect_origin);
        m_rect_origin = QRect();
        sizeMoveWindow();
    }
#else
    QMainWindow::showNormal();
#endif
}

void QGoodWindow::showMaximized()
{
#if defined Q_OS_WIN || defined Q_OS_LINUX
    if (m_fixed_size)
        return;
#endif
#ifdef Q_OS_WIN
    if (m_is_full_screen)
        showNormal();

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
    if (m_is_full_screen)
        showNormal();

    ShowWindow(m_hwnd, SW_SHOWMINIMIZED);
#else
    QMainWindow::showMinimized();
#endif
}

void QGoodWindow::showFullScreen()
{
#if defined Q_OS_WIN || defined Q_OS_LINUX
    if (m_fixed_size)
        return;
#endif
#ifdef Q_OS_WIN
    if (!m_is_full_screen)
    {
        ShowWindow(m_hwnd, SW_SHOWNORMAL);

        m_rect_origin = frameGeometry();

        SetWindowLongW(m_hwnd, GWL_STYLE, GetWindowLongW(m_hwnd, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);

        QTimer::singleShot(0, this, [=]{
            m_is_full_screen = true;
            sizeMoveWindow();
        });
    }
#else
    QMainWindow::showFullScreen();
#endif
}

void QGoodWindow::hide()
{
#ifdef Q_OS_WIN
    if (m_is_full_screen)
        showNormal();

    ShowWindow(m_hwnd, SW_HIDE);
#else
    QMainWindow::hide();
#endif
}

void QGoodWindow::close()
{
#ifdef Q_OS_WIN
    PostMessageW(m_hwnd, WM_CLOSE, 0, 0);
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

Qt::WindowStates QGoodWindow::windowState() const
{
#ifdef Q_OS_WIN
    return m_state;
#else
    return QMainWindow::windowState();
#endif
}

void QGoodWindow::setWindowState(Qt::WindowStates state)
{
#ifdef Q_OS_WIN
    if (state.testFlag(Qt::WindowFullScreen))
        showFullScreen();
    else if (state.testFlag(Qt::WindowMaximized))
        showMaximized();
    else if (state.testFlag(Qt::WindowMinimized))
        showMinimized();
    else if (state.testFlag(Qt::WindowNoState))
        showNormal();

    if (state.testFlag(Qt::WindowActive))
        activateWindow();
#else
    QMainWindow::setWindowState(state);
#endif
}

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

QString QGoodWindow::windowTitle() const
{
    return QMainWindow::windowTitle();
}

void QGoodWindow::setWindowTitle(const QString &title)
{
#ifdef Q_OS_WIN
    SetWindowTextW(m_hwnd, reinterpret_cast<WCHAR*>(const_cast<QChar*>(title.unicode())));

    QMainWindow::setWindowTitle(title);
#else
    QMainWindow::setWindowTitle(title);
#endif
}

QIcon QGoodWindow::windowIcon() const
{
    return QMainWindow::windowIcon();
}

void QGoodWindow::setWindowIcon(const QIcon &icon)
{
#ifdef Q_OS_WIN
    HICON hicon_big = QtWin::toHICON(icon.pixmap(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON)));

    HICON hicon_small = QtWin::toHICON(icon.pixmap(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON)));

    SendMessageW(m_hwnd, WM_SETICON, ICON_BIG, LPARAM(hicon_big));
    SendMessageW(m_hwnd, WM_SETICON, ICON_SMALL, LPARAM(hicon_small));

    QMainWindow::setWindowIcon(icon);
#else
    QMainWindow::setWindowIcon(icon);
#endif
}

bool QGoodWindow::event(QEvent *event)
{
#ifdef Q_OS_WIN
    switch (event->type())
    {
    case QEvent::ChildPolished:
    {
        QChildEvent *child_event = static_cast<QChildEvent*>(event);

        QDialog *dialog = qobject_cast<QDialog*>(child_event->child());

        if (!dialog)
            break;

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
#endif
#ifdef Q_OS_LINUX
    switch (event->type())
    {
    case QEvent::ChildAdded:
    {
        QChildEvent *child_event = static_cast<QChildEvent*>(event);

        QWidget *widget = qobject_cast<QWidget*>(child_event->child());

        if (!widget)
            break;

        if (widget->isWindow())
            break;

        widget->installEventFilter(this);
        widget->setMouseTracking(true);

        break;
    }
    case QEvent::WindowActivate:
    {
        QTimer::singleShot(0, this, &QGoodWindow::sizeMoveBorders);

        break;
    }
    case QEvent::Hide:
    case QEvent::WindowDeactivate:
    {
        for (const Shadow *shadow : m_shadow_list)
            const_cast<Shadow*>(shadow)->hide();

        break;
    }
    case QEvent::Resize:
    {
        setMask(rect());

        QTimer::singleShot(0, this, &QGoodWindow::sizeMoveBorders);

        break;
    }
    case QEvent::Move:
    {
        QTimer::singleShot(0, this, &QGoodWindow::sizeMoveBorders);

        break;
    }
    default:
        break;
    }
#endif
#ifdef Q_OS_MAC
    switch (event->type())
    {
    case QEvent::ChildAdded:
    {
        QChildEvent *child_event = static_cast<QChildEvent*>(event);

        QWidget *widget = qobject_cast<QWidget*>(child_event->child());

        if (!widget)
            break;

        if (widget->isWindow())
            break;

        widget->installEventFilter(this);
        widget->setMouseTracking(true);

        break;
    }
    default:
        break;
    }

#endif
    return QMainWindow::event(event);
}

bool QGoodWindow::eventFilter(QObject *watched, QEvent *event)
{
#ifdef Q_OS_WIN
    switch (event->type())
    {
    case QEvent::Show:
    {
        if (QDialog *dialog = qobject_cast<QDialog*>(watched))
        {
            //Center dialog.
            moveCenterDialog(dialog);
        }

        break;
    }
    default:
        break;
    }
#endif
#ifdef Q_OS_LINUX
    switch (event->type())
    {
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (mouse_event->button() == Qt::LeftButton)
        {
            int margin = m_margin;

            if (margin == TITLE_BAR)
            {
                if (!isMaximized())
                    showMaximized();
                else
                    showNormal();
            }
        }

        if (m_margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::MouseButtonPress:
    {
        setCursorForCurrentPos();

        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (!m_resize_move && mouse_event->button() == Qt::LeftButton)
        {
            if (m_margin != NO_WHERE)
                m_resize_move = true;
        }

        m_mouse_button_pressed = true;

        if (m_margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (m_resize_move && mouse_event->buttons() == Qt::LeftButton)
            sizeMove();
        else if (!m_mouse_button_pressed)
            setCursorForCurrentPos();

        if (m_margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (m_resize_move && mouse_event->button() == Qt::LeftButton)
        {
            if (m_margin != NO_WHERE)
                m_resize_move = false;
        }

        m_mouse_button_pressed = false;

        if (m_margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::Wheel:
    {
        if (m_margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::ContextMenu:
    {
        if (m_margin == TITLE_BAR)
            return true;

        break;
    }
    default:
        break;
    }
#endif
#ifdef Q_OS_MAC
    QPoint cursor_pos = QCursor::pos();
    int margin = ncHitTest(cursor_pos.x(), cursor_pos.y());

    switch (event->type())
    {
    case QEvent::MouseButtonDblClick:
    {
        if (margin == TITLE_BAR)
        {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

            if (!isFullScreen())
            {
                if (mouse_event->button() == Qt::LeftButton)
                {
                    if (margin == TITLE_BAR)
                    {
                        if (!isMaximized())
                            showMaximized();
                        else
                            showNormal();
                    }
                }
            }
        }

        if (margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::MouseButtonPress:
    {
        if (margin == TITLE_BAR)
        {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

            if (!isFullScreen())
            {
                if (mouse_event->button() == Qt::LeftButton)
                {
                    m_pos = cursor_pos - pos();
                }
            }
        }

        m_mouse_button_pressed = true;

        if (margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::MouseMove:
    {
        if (!m_pos.isNull())
        {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

            if (!isFullScreen())
            {
                if (mouse_event->buttons() == Qt::LeftButton)
                {
                    move(QCursor::pos() - m_pos);
                }
            }
        }

        if (!m_mouse_button_pressed)
        {
            if (margin == TITLE_BAR)
            {
                QApplication::setOverrideCursor(Qt::ArrowCursor);
            }
            else
            {
                QCursor cursor = static_cast<QWidget*>(watched)->cursor();
                QApplication::setOverrideCursor(cursor);
            }
        }

        if (margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::MouseButtonRelease:
    {
        if (!m_pos.isNull())
        {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

            if (!isFullScreen())
            {
                if (mouse_event->button() == Qt::LeftButton)
                    m_pos = QPoint();
            }
        }

        m_mouse_button_pressed = false;

        if (margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::Wheel:
    {
        if (margin == TITLE_BAR)
            return true;

        break;
    }
    case QEvent::ContextMenu:
    {
        if (margin == TITLE_BAR)
            return true;

        break;
    }
    default:
        break;
    }
#endif
    return QMainWindow::eventFilter(watched, event);
}

bool QGoodWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_LINUX
    if (eventType == "xcb_generic_event_t")
    {
        xcb_generic_event_t *event = static_cast<xcb_generic_event_t*>(message);

        if (event->response_type == XCB_GE_GENERIC)
        {
            if (m_resize_move_started)
            {
                m_resize_move_started = false;

                //Fix mouse problems after resize or move.
                QTest::mouseClick(windowHandle(), Qt::NoButton, Qt::NoModifier);
            }
            else
            {
                //Fix no mouse event after moving mouse from resize borders.
                QEvent event(QEvent::MouseMove);
                QApplication::sendEvent(this, &event);
            }
        }
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}

#ifdef Q_OS_WIN
LRESULT QGoodWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    QGoodWindow *gw = reinterpret_cast<QGoodWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    if (!gw)
        return DefWindowProcW(hwnd, message, wParam, lParam);

    switch (message)
    {
    case WM_ACTIVATE:
    {
        switch (wParam)
        {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
        {
            QTimer::singleShot(0, gw, [=]{
                gw->handleActivation();
            });

            break;
        }
        case WA_INACTIVE:
        {
            if (gw->window()->focusWidget())
            {
                //If app going to be inactive,
                //save current focused widget
                //to restore focus later.

                gw->m_focus_widget = gw->window()->focusWidget();
            }

            QTimer::singleShot(0, gw, [=]{
                gw->handleDeactivation();
            });

            break;
        }
        default:
            break;
        }

        break;
    }
    case WM_SETFOCUS:
    {
        QTimer::singleShot(0, gw, [=]{
            if (gw->m_focus_widget)
                gw->m_focus_widget->setFocus();
            else if (gw->m_win_widget)
                gw->m_win_widget->setFocus();
        });

        break;
    }
    case WM_SIZE:
    {
        gw->sizeMoveWindow();

        break;
    }
    case WM_MOVE:
    {
        gw->sizeMoveWindow();

        QMoveEvent event = QMoveEvent(QPoint(gw->x(), gw->y()), QPoint());
        QApplication::sendEvent(gw, &event);

        break;
    }
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);

        QSize minimum = gw->minimumSize();

        QSize sizeHint = gw->minimumSizeHint();

        mmi->ptMinTrackSize.x = qMax(minimum.width(), sizeHint.width());
        mmi->ptMinTrackSize.y = qMax(minimum.height(), sizeHint.height());

        QSize maximum = gw->maximumSize();

        mmi->ptMaxTrackSize.x = maximum.width();
        mmi->ptMaxTrackSize.y = maximum.height();

        break;
    }
    case WM_CLOSE:
    {
        //Send Qt QCloseEvent to the window,
        //which allows to accept or reject the window close.

        QCloseEvent event;
        QApplication::sendEvent(gw, &event);

        if (!event.isAccepted())
            return 0;

        gw->m_shadow->hide();

        gw->hide();

        //Do the needed cleanup.

        gw->m_main_window->setParent(nullptr);

        if (gw->m_win_widget)
            delete gw->m_win_widget;
        if (gw->m_shadow)
            delete gw->m_shadow;
        if (gw->m_helper_widget)
            delete gw->m_helper_widget;

        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));

        if (gw->testAttribute(Qt::WA_DeleteOnClose))
            delete gw;

        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    case WM_DESTROY:
    {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    case WM_NCHITTEST:
    {
        HRESULT lresult = HRESULT(gw->ncHitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));

        if (gw->m_is_full_screen)
        {
            //If on full screen, the whole window can be clicked.

            lresult = HTNOWHERE;

            return lresult;
        }

        if (gw->m_fixed_size)
        {
            //If have fixed size, then only HTCAPTION hit test is valid,
            //which means that only the title bar click and move is valid.

            if (lresult != HTNOWHERE && lresult != HTCAPTION)
                lresult = HTNOWHERE;

            return lresult;
        }

        return lresult;
    }
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    {
        if ((GetKeyState(VK_SHIFT) & 0x8000) && wParam == VK_F10)
        {
            //When SHIFT+F10 is pressed.
            gw->showContextMenu();
            return 0;
        }

        if ((GetKeyState(VK_MENU) & 0x8000) && wParam == VK_SPACE)
        {
            //When ALT+SPACE is pressed.
            gw->showContextMenu();
            return 0;
        }

        break;
    }
    case WM_NCRBUTTONUP:
    {
        //Show context menu on right click on title bar.

        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        LRESULT lRet = gw->ncHitTest(x, y);

        if (lRet == HTCAPTION || lRet == HTSYSMENU)
            gw->showContextMenu(x, y);

        break;
    }
    case WM_NCCALCSIZE:
    {
        if (gw->isFullScreen())
            break;

        if (gw->isMaximized())
        {
            //Compensate window client area when maximized,
            //by removing BORDERWIDTH value for all edges.

            const int border_width = BORDERWIDTH;

            InflateRect(reinterpret_cast<RECT*>(lParam), -border_width, -border_width);
        }

        //Make the whole window as client area.
        return 0;
    }
    case WM_NCPAINT:
    case WM_NCACTIVATE:
    {
        //Prevent undesired painting on window when DWM is not enabled.

        if (!QtWin::isCompositionEnabled())
            return TRUE;

        break;
    }
    case WM_WINDOWPOSCHANGING:
    {
        Qt::WindowState state;

        if (gw->isMinimized())
            state = Qt::WindowMinimized;
        else if (gw->isMaximized())
            state = Qt::WindowMaximized;
        else if (gw->isFullScreen())
            state = Qt::WindowFullScreen;
        else
            state = Qt::WindowNoState;

        if (state != gw->m_state)
        {
            //If window state changed.

            gw->m_state = state;

            QWindowStateChangeEvent event(state);
            QApplication::sendEvent(gw, &event);

            if (state != Qt::WindowNoState)
            {
                //Hide shadow if not on "normal" state.
                gw->m_shadow->hide();
            }
            else if (gw->isVisible())
            {
                //Show shadow if switching to "normal" state, with delay.
                gw->m_shadow->showLater();
            }

            if (state == Qt::WindowMinimized)
            {
                if (gw->window()->focusWidget())
                {
                    //If app going to be minimized,
                    //save current focused widget
                    //to restore focus later.

                    gw->m_focus_widget = gw->window()->focusWidget();
                }
            }
        }

        WINDOWPOS *pwp = reinterpret_cast<WINDOWPOS*>(lParam);

        if (pwp->flags & SWP_SHOWWINDOW)
        {
            QShowEvent event;
            QApplication::sendEvent(gw, &event);

            if (gw->m_fixed_size)
            {
                SetWindowLongW(hwnd, GWL_STYLE, GetWindowLongW(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
            }

            {
                //Restore brush with clear_color.

                HBRUSH brush = HBRUSH(CreateSolidBrush(RGB(gw->m_clear_color.red(),
                                                           gw->m_clear_color.green(),
                                                           gw->m_clear_color.blue())));
                HBRUSH oldbrush = HBRUSH(SetWindowLongPtrW(hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(brush)));

                DeleteObject(oldbrush);
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            if (state == Qt::WindowNoState)
            {
                //Show shadow if on "normal" state with delay.
                gw->m_shadow->showLater();
            }

            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            QTimer::singleShot(0, gw->m_win_widget, [=]{
                gw->m_win_widget->show();
                gw->m_win_widget->resize(0, 0);
                gw->m_win_widget->resize(gw->size());
            });
        }
        else if (pwp->flags & SWP_HIDEWINDOW)
        {
            if (gw->window()->focusWidget())
            {
                //If app have a valid focused widget,
                //save them to restore focus later.

                gw->m_focus_widget = gw->window()->focusWidget();
            }

            {
                //Set NULL brush.

                HBRUSH brush = HBRUSH(GetStockObject(NULL_BRUSH));
                HBRUSH oldbrush = HBRUSH(SetWindowLongPtrW(hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(brush)));

                DeleteObject(oldbrush);
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            QHideEvent event;
            QApplication::sendEvent(gw, &event);

            gw->m_shadow->hide();
        }
        else if (pwp->flags == (SWP_NOSIZE + SWP_NOMOVE))
        {
            //Activate window to fix no activation
            //problem when QGoodWindow isn't shown initially in
            //active state.

            if (state == Qt::WindowNoState)
                QTimer::singleShot(0, gw, &QGoodWindow::handleActivation);
        }

        break;
    }
    case WM_WINDOWPOSCHANGED:
    {
        if (!QtWin::isCompositionEnabled() && gw->isMaximized())
        {
            //Hack for prevent window goes to full screen when it's being maximized,
            //enable WS_CAPTION and schedule for disable it,
            //not mantaining WS_CAPTION all the time to prevent undesired painting on window
            //when title or icon of the window is changed when DWM is not enabled.

            gw->enableCaption();
            QTimer::singleShot(0, gw, &QGoodWindow::disableCaption);
        }

        break;
    }
    case WM_SETTEXT:
    {
        emit gw->windowTitleChanged(gw->windowTitle());
        break;
    }
    case WM_SETICON:
    {
        emit gw->windowIconChanged(gw->windowIcon());
        break;
    }
    case WM_DISPLAYCHANGE:
    {
        if (gw->isFullScreen())
            gw->showNormal();

        break;
    }
    case WM_SETTINGCHANGE:
    case WM_THEMECHANGED:
    case WM_DWMCOMPOSITIONCHANGED:
    {
        //Send the window change event to m_helper_widget,
        //this hack corrects the background color when switching between
        //Windows composition modes or system themes.
        SendMessageW(HWND(gw->m_helper_widget->winId()), message, 0, 0);

        if (QtWin::isCompositionEnabled())
        {
            QTimer::singleShot(500, gw, &QGoodWindow::enableCaption);
            QTimer::singleShot(750, gw, &QGoodWindow::frameChanged);

            QTimer::singleShot(1000, gw, [=]{
                SetWindowRgn(hwnd, nullptr, TRUE);
            });
        }
        else
        {
            QTimer::singleShot(500, gw, &QGoodWindow::disableCaption);
            QTimer::singleShot(750, gw, &QGoodWindow::frameChanged);
        }

        QTimer::singleShot(500, gw, &QGoodWindow::sizeMoveWindow);

        QTimer::singleShot(500, gw, [=]{
            gw->repaint();
        });

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

    long result = 0;

    bool return_value = gw->nativeEvent(QByteArray(), &msg, &result);

    if (return_value)
        return result;

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void QGoodWindow::handleActivation()
{
    if (!m_win_widget || !m_shadow)
        return;

    if (m_focus_widget)
    {
        //If have a previous m_focus_widget valid,
        //set the focus to this widget.
        if (!m_focus_widget->hasFocus())
            m_focus_widget->setFocus();
    }
    else
    {
        bool have_focusable_widget = false;

        for (const QWidget *next_focus : m_win_widget->findChildren<QWidget*>())
        {
            if (next_focus->focusPolicy() != Qt::NoFocus)
            {
                //Set focus to first focusable widget.
                have_focusable_widget = true;

                m_focus_widget = const_cast<QWidget*>(next_focus);

                QTimer::singleShot(0, m_focus_widget, [=]{
                    m_focus_widget->setFocus();
                });

                break;
            }
        }

        if (!have_focusable_widget)
        {
            //If not have focusable widget
            //set focus to m_win_widget.
            if (!m_win_widget->hasFocus())
                m_win_widget->setFocus();
        }
    }

    if (m_state == Qt::WindowNoState)
    {
        //If in "normal" state, make shadow visible.
        m_shadow->setActive(true);
        m_shadow->show();
    }

    if (!m_active_state)
    {
        m_active_state = true;

        QEvent event(QEvent::WindowActivate);
        QApplication::sendEvent(this, &event);
    }
}

void QGoodWindow::handleDeactivation()
{
    if (!m_win_widget || !m_shadow)
        return;

    m_shadow->setActive(false);

    if (m_active_state)
    {
        m_active_state = false;

        QEvent event(QEvent::WindowDeactivate);
        QApplication::sendEvent(this, &event);
    }
}

void QGoodWindow::enableCaption()
{
    SetWindowLongW(m_hwnd, GWL_STYLE, GetWindowLongW(m_hwnd, GWL_STYLE) | WS_CAPTION);
}

void QGoodWindow::disableCaption()
{
    SetWindowLongW(m_hwnd, GWL_STYLE, GetWindowLongW(m_hwnd, GWL_STYLE) & ~WS_CAPTION);
}

void QGoodWindow::frameChanged()
{
    SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void QGoodWindow::sizeMoveWindow()
{
    if (!m_win_widget || !m_shadow)
        return;

    for (const QScreen *screen : QApplication::screens())
    {
        if (screen->geometry().contains(frameGeometry().center()))
        {
            if (windowHandle()->screen() != screen)
                windowHandle()->setScreen(const_cast<QScreen*>(screen));
        }
    }

    if (isFullScreen())
    {
        QScreen *screen = windowHandle()->screen();
        setGeometry(screen->geometry());
    }

    if (!QtWin::isCompositionEnabled())
    {
        QRect window_rect = rect();

        const int border_width = BORDERWIDTH;

        if (isMaximized())
            window_rect.moveTopLeft(QPoint(border_width, border_width));

        SetWindowRgn(m_hwnd, QtWin::toHRGN(window_rect), TRUE);
    }

    m_win_widget->setGeometry(rect());

    if (m_state == Qt::WindowNoState)
    {
        const int shadow_width = m_shadow->shadowWidth();

        m_shadow->move(frameGeometry().x() - shadow_width, frameGeometry().y() - shadow_width);
        m_shadow->resize(frameGeometry().width() + shadow_width * 2, frameGeometry().height() + shadow_width * 2);

        m_shadow->setActive(isActiveWindow());
    }
}

LRESULT QGoodWindow::ncHitTest(int x, int y)
{
    const int borderWidth = (m_state == Qt::WindowNoState ? 1 : 0); //in pixels.
    int titleHeight = titleBarHeight(); //in pixels.

    //Get the point coordinates for the hit test.
    QPoint ptMouse = QPoint(x, y);

    //Get the window rectangle.
    QRect rcWindow = frameGeometry();

    //Determine if the hit test is for resizing. Default middle (1,1).
    int row = 1;
    int col = 1;
    bool onResizeBorder = false;

    //Determine if the point is at the top or bottom of the window.
    if (ptMouse.y() < rcWindow.top() + titleHeight)
    {
        onResizeBorder = (ptMouse.y() < (rcWindow.top() + borderWidth));
        row = 0;
    }
    else if (ptMouse.y() >= rcWindow.bottom() - borderWidth)
    {
        row = 2;
    }

    //Determine if the point is at the left or right of the window.
    if (ptMouse.x() < rcWindow.left() + borderWidth)
    {
        col = 0; //left side
    }
    else if (ptMouse.x() >= rcWindow.right() + 1 - borderWidth)
    {
        col = 2; //right side
    }
    else if (row == 0 && !onResizeBorder)
    {
        const QRegion left_mask = m_left_mask.translated(iconWidth(), 0);
        const QRegion right_mask = m_right_mask.translated(width() - rightMargin(), 0);

        QPoint ptMouseMap = m_win_widget->mapFromGlobal(ptMouse);

        if (ptMouse.x() >= rcWindow.right() + 1 - rightMargin())
        {
            if (right_mask.contains(ptMouseMap))
                return HTNOWHERE; //title bar buttons right
        }
        else if (ptMouse.x() > rcWindow.left() + iconWidth() &&
                 ptMouse.x() < rcWindow.left() + iconWidth() + leftMargin())
        {
            if (left_mask.contains(ptMouseMap))
                return HTNOWHERE; //custom title bar buttons left
        }
        else if (ptMouse.x() < rcWindow.left() + iconWidth())
        {
            return HTSYSMENU; //title bar icon
        }
    }

    //Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
    LRESULT hitTests[3][3] =
    {
        {HTTOPLEFT, onResizeBorder ? HTTOP : HTCAPTION, HTTOPRIGHT},
        {HTLEFT, HTNOWHERE, HTRIGHT},
        {HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT},
    };

    return hitTests[row][col];
}

void QGoodWindow::showContextMenu(int x, int y)
{
    HMENU hMenu = GetSystemMenu(m_hwnd, FALSE);

    if (!hMenu)
        return;

    MENUITEMINFOW mi;
    memset(&mi, 0, sizeof(MENUITEMINFOW));
    mi.cbSize = sizeof(MENUITEMINFOW);
    mi.fMask = MIIM_STATE;

    mi.fState = MF_ENABLED;

    SetMenuItemInfoW(hMenu, SC_RESTORE, FALSE, &mi);
    SetMenuItemInfoW(hMenu, SC_SIZE, FALSE, &mi);
    SetMenuItemInfoW(hMenu, SC_MOVE, FALSE, &mi);
    SetMenuItemInfoW(hMenu, SC_MAXIMIZE, FALSE, &mi);
    SetMenuItemInfoW(hMenu, SC_MINIMIZE, FALSE, &mi);

    mi.fState = MF_GRAYED;

    WINDOWPLACEMENT wp;
    GetWindowPlacement(m_hwnd, &wp);

    switch (wp.showCmd)
    {
    case SW_SHOWMAXIMIZED:
    {
        SetMenuItemInfoW(hMenu, SC_SIZE, FALSE, &mi);
        SetMenuItemInfoW(hMenu, SC_MOVE, FALSE, &mi);
        SetMenuItemInfoW(hMenu, SC_MAXIMIZE, FALSE, &mi);
        SetMenuDefaultItem(hMenu, SC_CLOSE, FALSE);
        break;
    }
    case SW_SHOWMINIMIZED:
    {
        SetMenuItemInfoW(hMenu, SC_MINIMIZE, FALSE, &mi);
        SetMenuDefaultItem(hMenu, SC_RESTORE, FALSE);
        break;
    }
    case SW_SHOWNORMAL:
    {
        SetMenuItemInfoW(hMenu, SC_RESTORE, FALSE, &mi);
        SetMenuDefaultItem(hMenu, SC_CLOSE, FALSE);
        break;
    }
    default:
        break;
    }

    if (!(GetWindowLongW(m_hwnd, GWL_STYLE) & WS_MAXIMIZEBOX))
    {
        SetMenuItemInfoW(hMenu, SC_MAXIMIZE, FALSE, &mi);
        SetMenuItemInfoW(hMenu, SC_RESTORE, FALSE, &mi);
    }

    if (!(GetWindowLongW(m_hwnd, GWL_STYLE) & WS_MINIMIZEBOX))
    {
        SetMenuItemInfoW(hMenu, SC_MINIMIZE, FALSE, &mi);
    }

    if (!(GetWindowLongW(m_hwnd, GWL_STYLE) & WS_SIZEBOX))
    {
        SetMenuItemInfoW(hMenu, SC_SIZE, FALSE, &mi);
    }

    BOOL cmd = TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, x, y, 0, m_hwnd, nullptr);

    if (cmd)
        PostMessageW(m_hwnd, WM_SYSCOMMAND, WPARAM(cmd), 0);
}

void QGoodWindow::showContextMenu()
{
    const int border_width = BORDERWIDTH;

    int x_pos = x() + (!isMaximized() ? border_width : 0);
    int y_pos = y() + titleBarHeight() - (isMaximized() ? border_width : 0);

    showContextMenu(x_pos, y_pos);
}

void QGoodWindow::moveCenterDialog(QDialog *dialog)
{
    const int title_bar_height = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
    const int border_width = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));

    QScreen *screen = windowHandle()->screen();
    QRect rect;

    if (!isMinimized() && isVisible())
        rect = frameGeometry();
    else
        rect = screen->availableGeometry();

    QRect screen_rect = screen->availableGeometry();

    QRect dialog_rect = dialog->geometry();

    dialog_rect.moveCenter(rect.center());

    dialog_rect.moveLeft(qMax(dialog_rect.left(), screen_rect.left() + border_width));
    dialog_rect.moveTop(qMax(dialog_rect.top() + titleBarHeight() / 2, screen_rect.top() + title_bar_height));
    dialog_rect.moveRight(qMin(dialog_rect.right(), screen_rect.right() - border_width));
    dialog_rect.moveBottom(qMin(dialog_rect.bottom(), screen_rect.bottom() - border_width));

    dialog->setGeometry(dialog_rect);

    if (dialog->windowIcon().isNull())
        dialog->setWindowIcon(windowIcon());

    dialog->setAttribute(Qt::WA_DontShowOnScreen, true);
    dialog->setAttribute(Qt::WA_DontShowOnScreen, false);

    if (isMinimized())
        showNormal();
}
#endif
#ifdef Q_OS_LINUX
void QGoodWindow::setCursorForCurrentPos()
{
    const QPoint cursor_pos = QCursor::pos();
    const int margin = ncHitTest(cursor_pos.x(), cursor_pos.y());

    m_cursor_pos = cursor_pos;
    m_margin = margin;

    QWidget *widget = QApplication::widgetAt(cursor_pos);

    if (!widget)
        return;

    Display *dpy = QX11Info::display();

    switch (margin)
    {
    case TOP_LEFT:
    {
        Cursor cursor = XCreateFontCursor(dpy, XC_top_left_corner);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case TOP:
    {
        Cursor cursor;

        if (!m_fixed_size)
            cursor = XCreateFontCursor(dpy, XC_top_side);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case TOP_RIGHT:
    {
        Cursor cursor;

        if (!m_fixed_size)
            cursor = XCreateFontCursor(dpy, XC_top_right_corner);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case LEFT:
    {
        Cursor cursor;

        if (!m_fixed_size)
            cursor = XCreateFontCursor(dpy, XC_left_side);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case RIGHT:
    {
        Cursor cursor;

        if (!m_fixed_size)
            cursor = XCreateFontCursor(dpy, XC_right_side);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case BOTTOM_LEFT:
    {
        Cursor cursor;

        if (!m_fixed_size)
            cursor = XCreateFontCursor(dpy, XC_bottom_left_corner);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case BOTTOM:
    {
        Cursor cursor;

        if (!m_fixed_size)
            cursor = XCreateFontCursor(dpy, XC_bottom_side);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case BOTTOM_RIGHT:
    {
        Cursor cursor;

        if (!m_fixed_size)
            cursor = XCreateFontCursor(dpy, XC_bottom_right_corner);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case TITLE_BAR:
    {
        QApplication::setOverrideCursor(Qt::ArrowCursor);

        break;
    }
    case NO_WHERE:
    {
        QCursor cursor = widget->cursor();
        QApplication::setOverrideCursor(cursor);

        break;
    }
    default:
        break;
    }
}

void QGoodWindow::startSystemMoveResize()
{
    const int margin = m_margin;

    if (margin == NO_WHERE)
        return;

    if (m_fixed_size && margin != TITLE_BAR)
        return;

    QPoint cursor_pos = m_cursor_pos;

    XClientMessageEvent xmsg;
    memset(&xmsg, 0, sizeof(XClientMessageEvent));

    xmsg.type = ClientMessage;
    xmsg.window = Window(winId());
    xmsg.message_type = XInternAtom(QX11Info::display(), "_NET_WM_MOVERESIZE", False);
    xmsg.format = 32;
    xmsg.data.l[0] = long(cursor_pos.x());
    xmsg.data.l[1] = long(cursor_pos.y());

    if (margin == TITLE_BAR)
        xmsg.data.l[2] = MOVERESIZE_MOVE;
    else
        xmsg.data.l[2] = long(margin);

    xmsg.data.l[3] = 0;
    xmsg.data.l[4] = 0;

    XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False,
               SubstructureRedirectMask | SubstructureNotifyMask,
               reinterpret_cast<XEvent*>(&xmsg));

    XUngrabPointer(QX11Info::display(), QX11Info::appTime());
    XFlush(QX11Info::display());

    QTimer::singleShot(500, this, [=]{
        m_resize_move_started = true;
    });
}

void QGoodWindow::sizeMove()
{
    if (!m_resize_move)
        return;

    m_resize_move = false;

    const int margin = m_margin;

    if (margin == TITLE_BAR)
    {
        QTimer::singleShot(0, this, [=]{
            startSystemMoveResize();
        });
    }
    else if (m_shadow_list.size() == SHADOW_WINDOW_COUNT)
    {
        startSystemMoveResize();
    }
}

void QGoodWindow::sizeMoveBorders()
{
    if (m_shadow_list.size() != SHADOW_WINDOW_COUNT)
        return;

    if (!windowState().testFlag(Qt::WindowNoState))
    {
        for (const Shadow *shadow : m_shadow_list)
            const_cast<Shadow*>(shadow)->hide();

        return;
    }

    const int border_width = qCeil(BORDERWIDTH * m_pixel_ratio);

    QRect geom = geometry();
    geom.adjust(-border_width, -border_width, border_width, border_width);

    QRect available_geom;

    QRegion available_rgn;

    for (const QScreen *screen : QApplication::screens())
    {
        available_rgn += screen->availableGeometry();
    }

    available_geom = available_rgn.boundingRect();

    {   //TOP LEFT
        Shadow *shadow = m_shadow_list.at(0);

        QRect geom1 = geom;
        geom1.moveTo(geom1.left(), geom1.top());
        geom1.setWidth(border_width);
        geom1.setHeight(border_width);

        if (available_geom.intersects(geom1))
        {
            shadow->setGeometry(geom1);
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()));
            shadow->show();
        }
        else
        {
            shadow->hide();
        }
    }

    {   //TOP RIGHT
        Shadow *shadow = m_shadow_list.at(1);

        QRect geom1 = geom;
        geom1.moveTo(geom1.right() - border_width + 1, geom1.top());
        geom1.setWidth(border_width);
        geom1.setHeight(border_width);

        if (available_geom.intersects(geom1))
        {
            shadow->setGeometry(geom1);
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()));
            shadow->show();
        }
        else
        {
            shadow->hide();
        }
    }

    {   //BOTTOM LEFT
        Shadow *shadow = m_shadow_list.at(2);

        QRect geom1 = geom;
        geom1.moveTo(geom1.left(), geom1.bottom() - border_width + 1);
        geom1.setWidth(border_width);
        geom1.setHeight(border_width);

        if (available_geom.intersects(geom1))
        {
            shadow->setGeometry(geom1);
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()));
            shadow->show();
        }
        else
        {
            shadow->hide();
        }
    }

    {   //BOTTOM RIGHT
        Shadow *shadow = m_shadow_list.at(3);

        QRect geom1 = geom;
        geom1.moveTo(geom1.right() - border_width + 1, geom1.bottom() - border_width + 1);
        geom1.setWidth(border_width);
        geom1.setHeight(border_width);

        if (available_geom.intersects(geom1))
        {
            shadow->setGeometry(geom1);
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()));
            shadow->show();
        }
        else
        {
            shadow->hide();
        }
    }

    {   //TOP
        Shadow *shadow = m_shadow_list.at(4);

        QRect geom1 = geom;
        geom1.moveTo(geom1.left() + border_width, geom1.top());
        geom1.setWidth(geom1.width() - border_width * 2);
        geom1.setHeight(border_width);

        if (available_geom.intersects(geom1))
        {
            shadow->setGeometry(geom1);
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()));
            shadow->show();
        }
        else
        {
            shadow->hide();
        }
    }

    {   //LEFT
        Shadow *shadow = m_shadow_list.at(5);

        QRect geom1 = geom;
        geom1.moveTo(geom1.left(), geom1.top() + border_width);
        geom1.setWidth(border_width);
        geom1.setHeight(geom1.height() - border_width * 2);

        if (available_geom.intersects(geom1))
        {
            shadow->setGeometry(geom1);
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()));
            shadow->show();
        }
        else
        {
            shadow->hide();
        }
    }

    {   //RIGHT
        Shadow *shadow = m_shadow_list.at(6);

        QRect geom1 = geom;
        geom1.moveTo(geom1.right() - border_width + 1, geom1.top() + border_width);
        geom1.setWidth(border_width);
        geom1.setHeight(geom1.height() - border_width * 2);

        if (available_geom.intersects(geom1))
        {
            shadow->setGeometry(geom1);
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()));
            shadow->show();
        }
        else
        {
            shadow->hide();
        }
    }

    {   //BOTTOM
        Shadow *shadow = m_shadow_list.at(7);

        QRect geom1 = geom;
        geom1.moveTo(geom1.left() + border_width, geom1.bottom() - border_width + 1);
        geom1.setWidth(geom1.width() - border_width * 2);
        geom1.setHeight(border_width);

        if (available_geom.intersects(geom1))
        {
            shadow->setGeometry(geom1);
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()));
            shadow->show();
        }
        else
        {
            shadow->hide();
        }
    }
}
#endif
#ifdef Q_OS_MAC
void QGoodWindow::notificationReceiver(const QByteArray &notification)
{
    if (notification == "NSWindowWillEnterFullScreenNotification")
        macOSNative::setStyle(long(winId()), true);
    else if (notification == "NSWindowDidExitFullScreenNotification")
        macOSNative::setStyle(long(winId()), false);
}
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
int QGoodWindow::ncHitTest(int x, int y)
{
    const int titleHeight = titleBarHeight(); //in pixels.

    //Get the point coordinates for the hit test.
    QPoint ptMouse = QPoint(x, y);

    //Get the window rectangle.
    QRect rcWindow = geometry();

    //Determine if the hit test is for resizing. Default middle (1,1).
    int row = 1;
    int col = 1;
    bool onResizeBorder = false;

    bool maximized = isMaximized();

    //Determine if the point is at the top or bottom of the window.
    if (ptMouse.y() < rcWindow.top() + titleHeight)
    {
        onResizeBorder = (ptMouse.y() < rcWindow.top());
        row = 0;
    }
    else if (!maximized && ptMouse.y() > rcWindow.bottom())
    {
        row = 2;
    }

    //Determine if the point is at the left or right of the window.
    if (!maximized && ptMouse.x() < rcWindow.left())
    {
        col = 0; //left side
    }
    else if (!maximized && ptMouse.x() > rcWindow.right())
    {
        col = 2; //right side
    }
    else if (row == 0 && !onResizeBorder)
    {
        const QRegion left_mask = m_left_mask.translated(iconWidth(), 0);
        const QRegion right_mask = m_right_mask.translated(width() - rightMargin(), 0);

        const QPoint ptMouseMap = mapFromGlobal(ptMouse);

        if (ptMouse.x() > rcWindow.right() - rightMargin())
        {
            if (right_mask.contains(ptMouseMap))
                return NO_WHERE; //custom title bar buttons right
        }
        else if (ptMouse.x() > rcWindow.left() + iconWidth() &&
                 ptMouse.x() < rcWindow.left() + iconWidth() + leftMargin())
        {
            if (left_mask.contains(ptMouseMap))
                return NO_WHERE; //custom title bar buttons left
        }
    }

    //Hit test (TOP_LEFT, ... BOTTOM_RIGHT)
    int hitTests[3][3] =
    {
        {TOP_LEFT, onResizeBorder ? TOP : TITLE_BAR, TOP_RIGHT},
        {LEFT, NO_WHERE, RIGHT},
        {BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT},
    };

    return hitTests[row][col];
}
#endif
