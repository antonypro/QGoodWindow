/*
The MIT License (MIT)

Copyright © 2018-2022 Antonio Dias

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

#if defined QGOODWINDOW && defined __linux__
#include <gtk/gtk.h>
#endif

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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtWinExtras>
#endif
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

namespace QGoodWindowUtils
{
class ParentWindow : public QWidget
{
public:
    explicit ParentWindow(QWidget *parent) : QWidget(parent, Qt::Window)
    {

    }

private:
    bool event(QEvent *event)
    {
        switch (event->type())
        {
        case QEvent::ChildRemoved:
        {
            delete this;
            return true;
        }
        default:
            break;
        }

        return QWidget::event(event);
    }
};

class NativeEventFilter : public QAbstractNativeEventFilter
{
public:
    NativeEventFilter(QGoodWindow *gw)
    {
        m_gw = gw;
        m_gw_hwnd = HWND(m_gw->winId());
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override
#else
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override
#endif
    {
        Q_UNUSED(eventType)

        MSG *msg = static_cast<MSG*>(message);

        if (!IsWindowVisible(msg->hwnd))
            return false;

        if (!IsChild(m_gw_hwnd, msg->hwnd))
            return false;

        switch (msg->message)
        {
        case WM_NCHITTEST:
        {
            QPoint pos = QPoint(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));

            HRESULT lresult = m_gw->ncHitTest(pos.x(), pos.y());

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
            //Pass to main window...

            if ((GetKeyState(VK_SHIFT) & 0x8000) && msg->wParam == VK_F10)
            {
                // ...when SHIFT+F10 is pressed.
                SendMessageW(m_gw_hwnd, msg->message, msg->wParam, msg->lParam);
                return true;
            }

            if ((GetKeyState(VK_MENU) & 0x8000) && msg->wParam == VK_SPACE)
            {
                // ...when ALT+SPACE is pressed.
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
    QPointer<QGoodWindow> m_gw;
    HWND m_gw_hwnd;
};

inline bool isWin11OrGreater()
{
    bool is_win_11_or_greater = false;

    typedef NTSTATUS(WINAPI *tRtlGetVersion)(LPOSVERSIONINFOEXW);
    tRtlGetVersion pRtlGetVersion = tRtlGetVersion(QLibrary::resolve("ntdll", "RtlGetVersion"));

    if (pRtlGetVersion)
    {
        OSVERSIONINFOEXW os_info;
        memset(&os_info, 0, sizeof(OSVERSIONINFOEXW));
        os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
        NTSTATUS status = pRtlGetVersion(&os_info);
        if (status == 0)
            is_win_11_or_greater = (os_info.dwMajorVersion >= 10 && os_info.dwBuildNumber >= 22000);
    }

    return is_win_11_or_greater;
}
}
#endif

#ifdef Q_OS_LINUX

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtX11Extras/QX11Info>
#include <QtTest/QtTest>
#else
#include <QtGui/private/qtx11extras_p.h>
#endif
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <xcb/xcb.h>

#define SHADOW_WINDOW_COUNT 8

namespace QGoodWindowUtils
{
QList<QGoodWindow*> m_gw_list;

GtkSettings *m_settings = nullptr;

void themeChangeNotification()
{
    for (QGoodWindow *gw : m_gw_list)
    {
        if (gw->m_theme_change_timer)
            gw->m_theme_change_timer->start();
    }
}

void registerThemeChangeNotification()
{
    if (!m_settings)
    {
        m_settings = gtk_settings_get_default();
        g_signal_connect(m_settings, "notify::gtk-theme-name", themeChangeNotification, nullptr);
    }
}
}
#endif

#ifdef Q_OS_MAC

#include "macosnative.h"

Notification notification;

namespace QGoodWindowUtils
{
bool m_theme_change_registered = false;
}
#endif

QGoodWindow::QGoodWindow(QWidget *parent, const QColor &clear_color) : QMainWindow(parent)
{
#ifdef QGOODWINDOW
    setParent(nullptr);

    m_dark = isSystemThemeDark();

    m_caption_buttons_handled = false;

    m_is_caption_button_pressed = false;
    m_last_caption_button_hovered = -1;
    m_caption_button_pressed = -1;

    m_theme_change_timer = new QTimer(this);
    connect(m_theme_change_timer, &QTimer::timeout, this, [=]{
        bool dark = isSystemThemeDark();

        if (m_dark != dark)
        {
            m_dark = dark;
            emit systemThemeChanged();
        }
    });
    m_theme_change_timer->setSingleShot(true);
    m_theme_change_timer->setInterval(0);

    m_hover_timer = new QTimer(this);
    m_hover_timer->setSingleShot(true);
    m_hover_timer->setInterval(300);
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
    m_last_move_button = -1;
#endif
#ifdef Q_OS_MAC
    m_mouse_button_pressed = false;
    m_on_animate_event = false;
#endif
#ifdef Q_OS_WIN
    m_closed = false;

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
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
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

    m_win_use_native_borders = QGoodWindowUtils::isWin11OrGreater();

    m_window_handle = QWindow::fromWinId(WId(m_hwnd));

    initGW();

    m_helper_widget = new QWidget();

    QScreen *screen = windowHandle()->screen();
    qreal pixel_ratio = screen->logicalDotsPerInch() / qreal(96);

    if (!m_win_use_native_borders)
        m_shadow = new Shadow(pixel_ratio, m_hwnd);

    m_main_window = static_cast<QMainWindow*>(this);
    m_main_window->installEventFilter(this);

    if (!m_native_event)
    {
        m_native_event = new QGoodWindowUtils::NativeEventFilter(this);
        qApp->installNativeEventFilter(m_native_event);
    }

    setMargins(30, 0, 0, 0);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!QtWin::isCompositionEnabled())
    {
        disableCaption();
        frameChanged();
    }
#endif

#endif
#ifdef Q_OS_LINUX
    m_resize_move = false;
    m_resize_move_started = false;

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    for (int i = 0; i < SHADOW_WINDOW_COUNT; i++)
    {
        Shadow *shadow = new Shadow(this);
        m_shadow_list.append(shadow);
    }

    installEventFilter(this);
    setMouseTracking(true);

    createWinId();

    QGoodWindowUtils::registerThemeChangeNotification();

    QGoodWindowUtils::m_gw_list.append(this);

    QScreen *screen = windowHandle()->screen();
    m_pixel_ratio = screen->logicalDotsPerInch() / qreal(96);

    setMargins(30, 0, 0, 0);
#endif
#ifdef Q_OS_MAC
    installEventFilter(this);
    setMouseTracking(true);

    createWinId();

    notification.addWindow(this);

    notification.registerNotification("NSWindowWillEnterFullScreenNotification", winId());
    notification.registerNotification("NSWindowDidExitFullScreenNotification", winId());

    if (!QGoodWindowUtils::m_theme_change_registered)
    {
        QGoodWindowUtils::m_theme_change_registered = true;
        macOSNative::registerThemeChangeNotification();
    }

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
#ifdef Q_OS_LINUX
    QGoodWindowUtils::m_gw_list.removeAll(this);
#endif
#ifdef Q_OS_MAC
    notification.removeWindow(this);
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
    removeEventFilter(this);
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

/*** QGOODWINDOW FUNCTIONS BEGIN ***/

bool QGoodWindow::isSystemThemeDark()
{
    bool dark = false;
#ifdef Q_OS_WIN
    typedef LONG(WINAPI *tRegGetValueW)(HKEY,LPCWSTR,LPCWSTR,DWORD,LPDWORD,PVOID,LPDWORD);
    tRegGetValueW pRegGetValueW = tRegGetValueW(QLibrary::resolve("advapi32", "RegGetValueW"));

    if (pRegGetValueW)
    {
        DWORD value;
        DWORD size = sizeof(value);
        if (pRegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                          L"AppsUseLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS)
            dark = (value == 0);
    }
#endif
#ifdef Q_OS_LINUX
    GtkSettings *settings = gtk_settings_get_default();
    gchar *theme_name;
    g_object_get(settings, "gtk-theme-name", &theme_name, nullptr);
    dark = QString(theme_name).endsWith("Dark", Qt::CaseInsensitive);
#endif
#ifdef Q_OS_MAC
    dark = QString(macOSNative::themeName()).endsWith("Dark", Qt::CaseInsensitive);
#endif
    return dark;
}

bool QGoodWindow::shouldBordersBeDrawnBySystem()
{
    bool drawn_by_system = true;
#ifdef Q_OS_WIN
    drawn_by_system = QGoodWindowUtils::isWin11OrGreater();
#endif
#ifdef Q_OS_LINUX
    drawn_by_system = false;
#endif
    return drawn_by_system;
}

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

    m_caption_buttons_handled = false;

    m_min_mask = QRegion();
    m_max_mask = QRegion();
    m_cls_mask = QRegion();

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

void QGoodWindow::setCaptionButtonsHandled(bool handled, const Qt::Corner &corner)
{
#ifdef QGOODWINDOW
    if (handled)
    {
        switch (corner)
        {
        case Qt::TopLeftCorner:
        case Qt::TopRightCorner:
        {
            m_caption_buttons_corner = corner;
            break;
        }
        default:
        {
            m_caption_buttons_corner = Qt::TopRightCorner;
            break;
        }
        }

        m_caption_buttons_handled = true;
    }
    else
    {
        m_caption_buttons_handled = false;

        m_min_mask = QRegion();
        m_max_mask = QRegion();
        m_cls_mask = QRegion();
    }
#else
    Q_UNUSED(handled)
    Q_UNUSED(corner)
#endif
}

void QGoodWindow::setMinimizeMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    if (m_caption_buttons_handled)
        m_min_mask = mask;
#else
    Q_UNUSED(mask)
#endif
}

void QGoodWindow::setMaximizeMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    if (m_caption_buttons_handled)
        m_max_mask = mask;
#else
    Q_UNUSED(mask)
#endif
}

void QGoodWindow::setCloseMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    if (m_caption_buttons_handled)
        m_cls_mask = mask;
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

QRect QGoodWindow::leftCaptionButtonsRect() const
{
#ifdef QGOODWINDOW
    return QRect(0, 0, m_left_margin, m_title_bar_height);
#else
    return QRect();
#endif
}

QRect QGoodWindow::rightCaptionButtonsRect() const
{
#ifdef QGOODWINDOW
    return QRect(0, 0, m_right_margin, m_title_bar_height);
#else
    return QRect();
#endif
}

/*** QGOODWINDOW FUNCTIONS END ***/

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

    QRect rect = QRect(window_rect.left, window_rect.top,
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
    RECT client_rect;
    GetClientRect(m_hwnd, &client_rect);

    QPoint window_pos = pos();

    QRect rect = QRect(window_pos.x() + client_rect.left,
                       window_pos.y() + client_rect.top,
                       client_rect.right - client_rect.left,
                       client_rect.bottom - client_rect.top);

    return rect;
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
    return geometry().width();
#else
    return QMainWindow::width();
#endif
}

int QGoodWindow::height() const
{
#ifdef Q_OS_WIN
    return geometry().height();
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
    if (m_win_use_native_borders)
    {
        const int border_width = BORDERWIDTH;
        width += border_width * 2;
        height += border_width;
    }

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

        m_rect_origin = geometry();

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
    SetWindowTextW(m_hwnd, reinterpret_cast<const WCHAR*>(title.utf16()));

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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    HICON hicon_big = QtWin::toHICON(icon.pixmap(GetSystemMetrics(SM_CXICON),
                                                 GetSystemMetrics(SM_CYICON)));
    HICON hicon_small = QtWin::toHICON(icon.pixmap(GetSystemMetrics(SM_CXSMICON),
                                                   GetSystemMetrics(SM_CYSMICON)));
#else
    HICON hicon_big = icon.pixmap(GetSystemMetrics(SM_CXICON),
                                  GetSystemMetrics(SM_CYICON)).toImage().toHICON();
    HICON hicon_small = icon.pixmap(GetSystemMetrics(SM_CXSMICON),
                                    GetSystemMetrics(SM_CYSMICON)).toImage().toHICON();
#endif

    SendMessageW(m_hwnd, WM_SETICON, ICON_BIG, LPARAM(hicon_big));
    SendMessageW(m_hwnd, WM_SETICON, ICON_SMALL, LPARAM(hicon_small));

    QMainWindow::setWindowIcon(icon);
#else
    QMainWindow::setWindowIcon(icon);
#endif
}

bool QGoodWindow::event(QEvent *event)
{
#if defined Q_OS_LINUX || defined Q_OS_MAC
    switch (event->type())
    {
    case QEvent::Leave:
    {
        buttonLeave(m_last_caption_button_hovered);
        m_last_move_button = -1;

        break;
    }
    case QEvent::Hide:
    case QEvent::HoverLeave:
    {
        if (QApplication::overrideCursor())
            QApplication::restoreOverrideCursor();

        break;
    }
    default:
        break;
    }
#endif
#ifdef Q_OS_WIN
    switch (event->type())
    {
    case QEvent::ChildPolished:
    {
        QChildEvent *child_event = static_cast<QChildEvent*>(event);

        QWidget *widget = qobject_cast<QWidget*>(child_event->child());

        if (!widget->isWindow())
            break;

        if (!widget->isModal())
            break;

        widget->adjustSize();

        widget->setParent(bestParentForModalWindow(), widget->windowFlags());

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
    case QEvent::WindowStateChange:
    {
        bool window_no_state = windowState().testFlag(Qt::WindowNoState);

        for (QSizeGrip *size_grip : findChildren<QSizeGrip*>())
        {
            if (!size_grip->window()->windowFlags().testFlag(Qt::SubWindow))
                size_grip->setVisible(window_no_state);
        }

        break;
    }
    case QEvent::Resize:
    {
        if (isVisible() && size() != m_main_window->size())
            resize(m_main_window->size());

        break;
    }
    case QEvent::Close:
    {
        if (m_closed)
        {
            hide();
            return true;
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
    case QEvent::WindowActivate:
    {
        QTimer::singleShot(0, this, &QGoodWindow::sizeMoveBorders);

        break;
    }
    case QEvent::Hide:
    case QEvent::WindowDeactivate:
    {
        for (Shadow *shadow : m_shadow_list)
            shadow->hide();

        break;
    }
    case QEvent::WindowStateChange:
    {
        if (!windowState().testFlag(Qt::WindowNoState))
        {
            for (Shadow *shadow : m_shadow_list)
                shadow->hide();
        }

        break;
    }
    case QEvent::Show:
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
    case QEvent::WindowBlocked:
    {
        setEnabled(false);

        m_last_fixed_size_value = m_fixed_size;

        setFixedSize(size());

        if (QWidget *w = qApp->activeModalWidget())
            w->setEnabled(true);

        break;
    }
    case QEvent::WindowUnblocked:
    {
        setEnabled(true);

        if (!m_last_fixed_size_value)
        {
            setMinimumSize(0, 0);
            setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

            m_fixed_size = false;
        }

        break;
    }
    default:
        break;
    }
#endif
#ifdef Q_OS_MAC
    switch (event->type())
    {
    case QEvent::Show:
    {
        if (isFullScreen())
            break;

        macOSNative::setStyle(long(winId()), false);

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
    if (watched == m_main_window)
    {
        switch (event->type())
        {
        case QEvent::WindowActivate:
        {
            if (!m_main_window->isActiveWindow() || m_active_state)
                return true;

            m_active_state = true;

            break;
        }
        case QEvent::WindowDeactivate:
        {
            if (m_main_window->isActiveWindow() || !m_active_state)
                return true;

            m_active_state = false;

            break;
        }
        default:
            break;
        }
    }
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
    if (!isEnabled())
        return QMainWindow::eventFilter(watched, event);

    QPoint cursor_pos = QCursor::pos();
    long button = ncHitTest(cursor_pos.x(), cursor_pos.y());

    switch (event->type())
    {
#ifdef Q_OS_MAC
    case QEvent::ChildAdded:
    case QEvent::ChildRemoved:
    {
        if (qApp->activeModalWidget())
            break;

        if (isFullScreen())
            break;

        if (m_on_animate_event)
            break;

        macOSNative::setStyle(long(winId()), false);

        break;
    }
#endif
    case QEvent::ChildPolished:
    {
        QChildEvent *child_event = static_cast<QChildEvent*>(event);

        QWidget *widget = qobject_cast<QWidget*>(child_event->child());

        if (!widget)
            break;

        widget->setMouseTracking(true);
        widget->installEventFilter(this);

        for (QWidget *w : widget->findChildren<QWidget*>())
        {
            w->setMouseTracking(true);
            w->installEventFilter(this);
        }

        break;
    }
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (mouse_event->button() != Qt::LeftButton)
            break;

        switch (button)
        {
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
        {
            buttonPress(button);
            break;
        }
        default:
            break;
        }

        break;
    }
    case QEvent::MouseMove:
    {
        if (m_last_move_button != button)
        {
            m_last_move_button = button;

            buttonLeave(m_last_caption_button_hovered);

            switch (button)
            {
            case HTMINBUTTON:
            case HTMAXBUTTON:
            case HTCLOSE:
            {
                if (!m_is_caption_button_pressed || (button == m_caption_button_pressed))
                    buttonEnter(button);

                break;
            }
            default:
                break;
            }
        }

        break;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (mouse_event->button() != Qt::LeftButton)
            break;

        m_last_move_button = -1;

        buttonRelease(m_caption_button_pressed, (button == m_caption_button_pressed));

        break;
    }
    default:
        break;
    }
#endif
#ifdef Q_OS_LINUX
    if (m_is_caption_button_pressed)
        return QMainWindow::eventFilter(watched, event);

    switch (event->type())
    {
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (mouse_event->button() == Qt::LeftButton)
        {
            if (m_margin == TITLE_BAR)
            {
                if (!isMaximized())
                    showMaximized();
                else
                    showNormal();
            }
        }

        switch (m_margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

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

        switch (m_margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

        break;
    }
    case QEvent::MouseMove:
    {
        setCursorForCurrentPos();

        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (m_resize_move && mouse_event->buttons() == Qt::LeftButton)
            sizeMove();

        switch (m_margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

        break;
    }
    case QEvent::MouseButtonRelease:
    {
        setCursorForCurrentPos();

        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (m_resize_move && mouse_event->button() == Qt::LeftButton)
        {
            if (m_margin != NO_WHERE)
                m_resize_move = false;
        }

        switch (m_margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

        break;
    }
    case QEvent::Wheel:
    {
        switch (m_margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

        break;
    }
    case QEvent::ContextMenu:
    {
        switch (m_margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

        break;
    }
    default:
        break;
    }
#endif
#ifdef Q_OS_MAC
    if (m_is_caption_button_pressed)
        return QMainWindow::eventFilter(watched, event);

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

        switch (margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

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

        switch (margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

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
                    move(cursor_pos - m_pos);
                }
            }
        }

        if (!m_mouse_button_pressed)
        {
            QWidget *widget = QApplication::widgetAt(QCursor::pos());

            if (widget)
            {
                switch (margin)
                {
                case TITLE_BAR:
                case HTMINBUTTON:
                case HTMAXBUTTON:
                case HTCLOSE:
                {
                    if (QApplication::overrideCursor() &&
                            QApplication::overrideCursor()->shape() == Qt::ArrowCursor)
                        break;

                    QApplication::setOverrideCursor(Qt::ArrowCursor);

                    break;
                }
                case NO_WHERE:
                {
                    if (!QApplication::overrideCursor())
                        break;

                    if (QApplication::overrideCursor()->shape() != Qt::ArrowCursor)
                        break;

                    QApplication::restoreOverrideCursor();

                    break;
                }
                default:
                    break;
                }
            }
        }

        switch (margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

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

        switch (margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

        break;
    }
    case QEvent::Wheel:
    {
        switch (margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

        break;
    }
    case QEvent::ContextMenu:
    {
        switch (margin)
        {
        case TITLE_BAR:
        case HTMINBUTTON:
        case HTMAXBUTTON:
        case HTCLOSE:
            return true;
        default:
            break;
        }

        break;
    }
    default:
        break;
    }
#endif
    return QMainWindow::eventFilter(watched, event);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
bool QGoodWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
#else
bool QGoodWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#endif
{
#ifdef Q_OS_LINUX
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
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
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}

#ifdef Q_OS_WIN
void QGoodWindow::initGW()
{
    setProperty("_q_embedded_native_parent_handle", WId(m_hwnd));
    setWindowFlags(Qt::FramelessWindowHint);

    QEvent event(QEvent::EmbeddingControl);
    QApplication::sendEvent(this, &event);
}

void QGoodWindow::destroyGW()
{
    QMainWindow::close();
    QMainWindow::destroy();
}

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
                gw->setWidgetFocus();
                gw->handleActivation();
            });

            break;
        }
        case WA_INACTIVE:
        {
            if (gw->focusWidget())
            {
                //If app going to be inactive,
                //save current focused widget
                //to restore focus later.

                gw->m_focus_widget = gw->focusWidget();
            }

            QTimer::singleShot(0, gw, [=]{
                gw->handleDeactivation();
            });

            if (!IsWindowEnabled(hwnd))
            {
                if (gw->m_shadow)
                    gw->m_shadow->hide();
            }

            break;
        }
        default:
            break;
        }

        break;
    }
    case WM_SETFOCUS:
    {
        //Pass focus to last focused widget.
        gw->setWidgetFocus();
        gw->m_main_window->activateWindow();

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

        QMoveEvent event(QPoint(gw->x(), gw->y()), QPoint());
        QApplication::sendEvent(gw, &event);

        break;
    }
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO *mmi = reinterpret_cast<MINMAXINFO*>(lParam);

        QSize minimum = gw->minimumSize();

        QSize sizeHint = gw->minimumSizeHint();

        const int border_width = BORDERWIDTH;

        mmi->ptMinTrackSize.x = qMax(minimum.width(), sizeHint.width()) +
                (gw->m_win_use_native_borders ? border_width * 2 : 0);
        mmi->ptMinTrackSize.y = qMax(minimum.height(), sizeHint.height()) +
                (gw->m_win_use_native_borders ? border_width : 0);

        QSize maximum = gw->maximumSize();

        mmi->ptMaxTrackSize.x = maximum.width() + (gw->m_win_use_native_borders ? border_width * 2 : 0);
        mmi->ptMaxTrackSize.y = maximum.height() + (gw->m_win_use_native_borders ? border_width : 0);

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

        gw->m_closed = true;

        //Do the needed cleanup.
        gw->destroyGW();

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
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        LRESULT lresult = LRESULT(gw->ncHitTest(pos.x(), pos.y()));

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
    case WM_NCMOUSELEAVE:
    {
        if (gw->m_is_caption_button_pressed)
            break;

        gw->buttonLeave(gw->m_last_caption_button_hovered);

        break;
    }
    case WM_NCMOUSEMOVE:
    {
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        long button = gw->ncHitTest(pos.x(), pos.y());

        bool valid_caption_button = gw->winButtonHover(button);

        if (valid_caption_button)
        {
            if (gw->m_is_caption_button_pressed)
            {
                if (GetCapture() != hwnd)
                {
                    SetCapture(hwnd);
                }
            }
        }
        else
        {
            gw->buttonLeave(gw->m_last_caption_button_hovered);
        }

        break;
    }
    case WM_MOUSEMOVE:
    {
        QPoint pos = gw->mapToGlobal(QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));

        long button = gw->ncHitTest(pos.x(), pos.y());

        bool valid_caption_button = gw->winButtonHover(button);

        if (!valid_caption_button)
        {
            if (!gw->m_is_caption_button_pressed && GetCapture() == hwnd)
            {
                ReleaseCapture();
            }

            gw->buttonLeave(gw->m_last_caption_button_hovered);
        }

        break;
    }
    case WM_NCLBUTTONDOWN:
    {
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        long button = gw->ncHitTest(pos.x(), pos.y());

        bool valid_caption_button = gw->buttonPress(button);

        if (valid_caption_button)
            return 0;

        break;
    }
    case WM_NCLBUTTONUP:
    {
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        long button = gw->ncHitTest(pos.x(), pos.y());

        bool valid_caption_button = gw->buttonRelease(gw->m_caption_button_pressed,
                                                      (button == gw->m_caption_button_pressed));

        if (valid_caption_button)
            return 0;

        break;
    }
    case WM_LBUTTONDOWN:
    {
        QPoint pos = gw->mapToGlobal(QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));

        long button = gw->ncHitTest(pos.x(), pos.y());

        gw->buttonPress(button);

        break;
    }
    case WM_LBUTTONUP:
    {
        QPoint pos = gw->mapToGlobal(QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));

        long button = gw->ncHitTest(pos.x(), pos.y());

        if (button != gw->m_caption_button_pressed)
            gw->buttonEnter(button);

        gw->buttonRelease(gw->m_caption_button_pressed, (button == gw->m_caption_button_pressed));

        break;
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

        if (gw->m_win_use_native_borders && !gw->isMaximized())
        {
            const int border_width = BORDERWIDTH;

            RECT *rect = reinterpret_cast<RECT*>(lParam);
            rect->left += border_width;
            rect->bottom -= border_width;
            rect->right -= border_width;
        }

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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    case WM_NCPAINT:
    case WM_NCACTIVATE:
    {
        //Prevent undesired painting on window when DWM is not enabled.

        if (!QtWin::isCompositionEnabled())
            return TRUE;

        break;
    }
#endif
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

            if (gw->m_shadow)
            {
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
            }

            if (state == Qt::WindowMinimized)
            {
                if (gw->focusWidget())
                {
                    //If app going to be minimized,
                    //save current focused widget
                    //to restore focus later.

                    gw->m_focus_widget = gw->focusWidget();
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
                HBRUSH oldbrush = HBRUSH(SetWindowLongPtrW(hwnd, GCLP_HBRBACKGROUND,
                                                           reinterpret_cast<LONG_PTR>(brush)));

                DeleteObject(oldbrush);
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            if (gw->m_shadow)
            {
                if (state == Qt::WindowNoState)
                {
                    //Show shadow if on "normal" state with delay.
                    gw->m_shadow->showLater();
                }
            }

            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            QTimer::singleShot(0, gw->m_main_window, [=]{
                if (gw->m_main_window->isVisible())
                    return;

                //Fix bug that make the window frozen
                //when initializing window.
                QWidget *central_widget = gw->centralWidget();
                if (central_widget) central_widget->hide();
                gw->m_main_window->show();
                gw->m_main_window->resize(gw->size());
                if (central_widget) central_widget->show();
            });
        }
        else if (pwp->flags & SWP_HIDEWINDOW)
        {
            if (gw->focusWidget())
            {
                //If app have a valid focused widget,
                //save them to restore focus later.

                gw->m_focus_widget = gw->focusWidget();
            }

            {
                //Set NULL brush.

                HBRUSH brush = HBRUSH(GetStockObject(NULL_BRUSH));
                HBRUSH oldbrush = HBRUSH(SetWindowLongPtrW(hwnd, GCLP_HBRBACKGROUND,
                                                           reinterpret_cast<LONG_PTR>(brush)));

                DeleteObject(oldbrush);
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            QHideEvent event;
            QApplication::sendEvent(gw, &event);

            if (gw->m_shadow)
                gw->m_shadow->hide();
        }
        else if (pwp->flags == (SWP_NOSIZE + SWP_NOMOVE))
        {
            //Activate window to fix no activation
            //problem when QGoodWindow isn't shown initially in
            //active state.

            gw->m_main_window->activateWindow();

            if (gw->m_shadow)
                gw->m_shadow->showLater();
        }

        break;
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
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
#endif
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
    {
        if (gw->m_theme_change_timer)
        {
            if (QString::fromWCharArray(LPCWSTR(lParam)) == "ImmersiveColorSet")
            {
                gw->m_theme_change_timer->start();
            }
        }

        break;
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
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
#endif
    default:
        break;
    }

    MSG msg;
    msg.hwnd = hwnd;
    msg.message = message;
    msg.lParam = lParam;
    msg.wParam = wParam;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    long result = 0;
#else
    qintptr result = 0;
#endif

    bool return_value = gw->nativeEvent(QByteArray(), &msg, &result);

    if (return_value)
        return result;

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void QGoodWindow::handleActivation()
{
    if (m_shadow)
    {
        if (m_state == Qt::WindowNoState)
        {
            //If in "normal" state, make shadow visible.
            m_shadow->setActive(true);
            m_shadow->show();
        }
    }
}

void QGoodWindow::handleDeactivation()
{
    if (m_shadow)
        m_shadow->setActive(false);

    if (!isEnabled())
        buttonLeave(m_last_caption_button_hovered);
}

void QGoodWindow::setWidgetFocus()
{
    if (!m_focus_widget)
    {
        bool have_focusable_widget = false;

        for (QWidget *next_focus : findChildren<QWidget*>())
        {
            if (next_focus->focusPolicy() != Qt::NoFocus)
            {
                //Set focus to first focusable widget.
                have_focusable_widget = true;
                m_focus_widget = next_focus;
                break;
            }
        }

        if (!have_focusable_widget)
        {
            //If not have focusable widget
            //set focus to m_main_window.
            m_focus_widget = m_main_window;
        }
    }

    if (m_focus_widget)
    {
        //If have a valid m_focus_widget,
        //set the focus to this widget
        m_focus_widget->setFocus();
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
    for (QScreen *screen : QApplication::screens())
    {
        if (screen->geometry().contains(frameGeometry().center()))
        {
            if (windowHandle()->screen() != screen)
                windowHandle()->setScreen(screen);
        }
    }

    if (isFullScreen())
    {
        QScreen *screen = windowHandle()->screen();
        setGeometry(screen->geometry());
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!QtWin::isCompositionEnabled())
    {
        QRect window_rect = rect();

        const int border_width = BORDERWIDTH;

        if (isMaximized())
            window_rect.moveTopLeft(QPoint(border_width, border_width));

        SetWindowRgn(m_hwnd, QtWin::toHRGN(window_rect), TRUE);
    }
#endif

    m_main_window->setGeometry(rect());

    if (m_shadow)
    {
        if (m_state == Qt::WindowNoState)
        {
            const int shadow_width = m_shadow->shadowWidth();

            m_shadow->move(x() - shadow_width, y() - shadow_width);
            m_shadow->resize(width() + shadow_width * 2, height() + shadow_width * 2);

            m_shadow->setActive(isActiveWindow());
        }
    }
}

LRESULT QGoodWindow::ncHitTest(int x, int y)
{
    for (QSizeGrip *size_grip : findChildren<QSizeGrip*>())
    {
        QPoint pos = QPoint(x, y);

        if (size_grip->isEnabled() &&
                !size_grip->window()->windowFlags().testFlag(Qt::SubWindow))
        {
            if (size_grip->rect().contains(size_grip->mapFromGlobal(pos)))
                return HTBOTTOMRIGHT;
        }
    }

    if (!m_win_use_native_borders)
    {
        const int border_width = (m_state == Qt::WindowNoState ? 1 : 0); //in pixels.
        int title_height = titleBarHeight(); //in pixels.

        //Get the point coordinates for the hit test.
        QPoint mouse_pos = QPoint(x, y);

        //Get the window rectangle.
        QRect window_rect = frameGeometry();

        //Determine if the hit test is for resizing. Default middle (1,1).
        int row = 1;
        int col = 1;
        bool on_resize_border = false;

        //Determine if the point is at the top or bottom of the window.
        if (mouse_pos.y() < window_rect.top() + title_height)
        {
            on_resize_border = (mouse_pos.y() < (window_rect.top() + border_width));
            row = 0;
        }
        else if (mouse_pos.y() >= window_rect.bottom() - border_width)
        {
            row = 2;
        }

        //Determine if the point is at the left or right of the window.
        if (mouse_pos.x() < window_rect.left() + border_width)
        {
            col = 0; //left side
        }
        else if (mouse_pos.x() > window_rect.right())
        {
            col = 2; //right side
        }
        else if (row == 0 && !on_resize_border)
        {
            const QRegion left_mask = m_left_mask.translated(iconWidth(), 0);
            const QRegion right_mask = m_right_mask.translated(width() - rightMargin(), 0);

            QPoint mouse_pos_map = mapFromGlobal(mouse_pos);

            if (mouse_pos.x() > window_rect.right() - rightMargin())
            {
                if (m_caption_buttons_handled && m_caption_buttons_corner == Qt::TopRightCorner)
                {
                    QPoint pos = mouse_pos_map;
                    pos.setX(pos.x() - (window_rect.right() - window_rect.left() - rightMargin()));

                    if (m_cls_mask.contains(pos))
                        return HTCLOSE; //title bar close button
                    else if (m_max_mask.contains(pos))
                        return HTMAXBUTTON; //title bar maximize button
                    else if (m_min_mask.contains(pos))
                        return HTMINBUTTON; //title bar minimize button
                }

                if (right_mask.contains(mouse_pos_map))
                {
                    return HTNOWHERE; //title bar buttons right
                }
            }
            else if (mouse_pos.x() > window_rect.left() + iconWidth() &&
                     mouse_pos.x() < window_rect.left() + iconWidth() + leftMargin())
            {
                if (m_caption_buttons_handled && m_caption_buttons_corner == Qt::TopLeftCorner)
                {
                    QPoint pos = mouse_pos_map;
                    pos.setX(pos.x() - iconWidth());

                    if (m_cls_mask.contains(pos))
                        return HTCLOSE; //title bar close button
                    else if (m_max_mask.contains(pos))
                        return HTMAXBUTTON; //title bar maximize button
                    else if (m_min_mask.contains(pos))
                        return HTMINBUTTON; //title bar minimize button
                }

                if (left_mask.contains(mouse_pos_map))
                {
                    return HTNOWHERE; //custom title bar buttons left
                }
            }
            else if (mouse_pos.x() < window_rect.left() + iconWidth())
            {
                return HTSYSMENU; //title bar icon
            }
        }

        //Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
        LRESULT hitTests[3][3] =
        {
            {HTTOPLEFT, on_resize_border ? HTTOP : HTCAPTION, HTTOPRIGHT},
            {HTLEFT, HTNOWHERE, HTRIGHT},
            {HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT},
        };

        return hitTests[row][col];
    }
    else
    {
        const int border_width = (m_state == Qt::WindowNoState ? BORDERWIDTH : 0); //in pixels.
        int title_height = titleBarHeight(); //in pixels.

        //Get the point coordinates for the hit test.
        QPoint mouse_pos = QPoint(x, y);

        //Get the window rectangle.
        QRect window_rect = geometry();

        //Determine if the hit test is for resizing. Default middle (1,1).
        int row = 1;
        int col = 1;
        bool on_resize_border = false;

        //Determine if the point is at the top or bottom of the window.
        if (mouse_pos.y() < window_rect.top() + title_height)
        {
            on_resize_border = (mouse_pos.y() < (window_rect.top() + border_width) &&
                              mouse_pos.x() > (window_rect.left() + border_width) &&
                              mouse_pos.x() < (window_rect.right() + border_width * 2));
            row = 0;
        }
        else if (mouse_pos.y() >= window_rect.bottom())
        {
            row = 2;
        }

        //Determine if the point is at the left or right of the window.
        if (mouse_pos.x() < window_rect.left() + border_width)
        {
            col = 0; //left side
        }
        else if (mouse_pos.x() > window_rect.right() + border_width)
        {
            col = 2; //right side
        }
        else if (row == 0 && !on_resize_border)
        {
            const QRegion left_mask = m_left_mask.translated(iconWidth(), 0);
            const QRegion right_mask = m_right_mask.translated(width() - rightMargin(), 0);

            QPoint mouse_pos_map = mapFromGlobal(mouse_pos);

            if (mouse_pos.x() > window_rect.right() - rightMargin() + border_width)
            {
                if (m_caption_buttons_handled && m_caption_buttons_corner == Qt::TopRightCorner)
                {
                    QPoint pos = mouse_pos_map;
                    pos.setX(pos.x() - (window_rect.right() - window_rect.left() - rightMargin()));

                    if (m_cls_mask.contains(pos))
                        return HTCLOSE; //title bar close button
                    else if (m_max_mask.contains(pos))
                        return HTMAXBUTTON; //title bar maximize button
                    else if (m_min_mask.contains(pos))
                        return HTMINBUTTON; //title bar minimize button
                }

                if (right_mask.contains(mouse_pos_map))
                {
                    return HTNOWHERE; //title bar buttons right
                }
            }
            else if (mouse_pos.x() > window_rect.left() + iconWidth() &&
                     mouse_pos.x() < window_rect.left() + iconWidth() + leftMargin())
            {
                if (m_caption_buttons_handled && m_caption_buttons_corner == Qt::TopLeftCorner)
                {
                    QPoint pos = mouse_pos_map;
                    pos.setX(pos.x() - iconWidth());

                    if (m_cls_mask.contains(pos))
                        return HTCLOSE; //title bar close button
                    else if (m_max_mask.contains(pos))
                        return HTMAXBUTTON; //title bar maximize button
                    else if (m_min_mask.contains(pos))
                        return HTMINBUTTON; //title bar minimize button
                }

                if (left_mask.contains(mouse_pos_map))
                {
                    return HTNOWHERE; //custom title bar buttons left
                }
            }
            else if (mouse_pos.x() < window_rect.left() + iconWidth())
            {
                return HTSYSMENU; //title bar icon
            }
        }

        //Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
        LRESULT hitTests[3][3] =
        {
            {HTTOPLEFT, on_resize_border ? HTTOP : HTCAPTION, HTTOPRIGHT},
            {HTLEFT, HTNOWHERE, HTRIGHT},
            {HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT},
        };

        return hitTests[row][col];
    }
}

void QGoodWindow::showContextMenu(int x, int y)
{
    HMENU menu = GetSystemMenu(m_hwnd, FALSE);

    if (!menu)
        return;

    MENUITEMINFOW mi;
    memset(&mi, 0, sizeof(MENUITEMINFOW));
    mi.cbSize = sizeof(MENUITEMINFOW);
    mi.fMask = MIIM_STATE;

    mi.fState = MF_ENABLED;

    SetMenuItemInfoW(menu, SC_RESTORE, FALSE, &mi);
    SetMenuItemInfoW(menu, SC_SIZE, FALSE, &mi);
    SetMenuItemInfoW(menu, SC_MOVE, FALSE, &mi);
    SetMenuItemInfoW(menu, SC_MAXIMIZE, FALSE, &mi);
    SetMenuItemInfoW(menu, SC_MINIMIZE, FALSE, &mi);

    mi.fState = MF_GRAYED;

    WINDOWPLACEMENT wp;
    GetWindowPlacement(m_hwnd, &wp);

    switch (wp.showCmd)
    {
    case SW_SHOWMAXIMIZED:
    {
        SetMenuItemInfoW(menu, SC_SIZE, FALSE, &mi);
        SetMenuItemInfoW(menu, SC_MOVE, FALSE, &mi);
        SetMenuItemInfoW(menu, SC_MAXIMIZE, FALSE, &mi);
        SetMenuDefaultItem(menu, SC_CLOSE, FALSE);
        break;
    }
    case SW_SHOWMINIMIZED:
    {
        SetMenuItemInfoW(menu, SC_MINIMIZE, FALSE, &mi);
        SetMenuDefaultItem(menu, SC_RESTORE, FALSE);
        break;
    }
    case SW_SHOWNORMAL:
    {
        SetMenuItemInfoW(menu, SC_RESTORE, FALSE, &mi);
        SetMenuDefaultItem(menu, SC_CLOSE, FALSE);
        break;
    }
    default:
        break;
    }

    if (!(GetWindowLongW(m_hwnd, GWL_STYLE) & WS_MAXIMIZEBOX))
    {
        SetMenuItemInfoW(menu, SC_MAXIMIZE, FALSE, &mi);
        SetMenuItemInfoW(menu, SC_RESTORE, FALSE, &mi);
    }

    if (!(GetWindowLongW(m_hwnd, GWL_STYLE) & WS_MINIMIZEBOX))
    {
        SetMenuItemInfoW(menu, SC_MINIMIZE, FALSE, &mi);
    }

    if (!(GetWindowLongW(m_hwnd, GWL_STYLE) & WS_SIZEBOX))
    {
        SetMenuItemInfoW(menu, SC_SIZE, FALSE, &mi);
    }

    int cmd = int(TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
                                 x, y, 0, m_hwnd, nullptr));

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

QWidget *QGoodWindow::bestParentForModalWindow()
{
    QGoodWindowUtils::ParentWindow *parent = new QGoodWindowUtils::ParentWindow(this);
    moveCenterWindow(parent);

    return parent;
}

void QGoodWindow::moveCenterWindow(QWidget *widget)
{
    const int title_bar_height = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
                                  GetSystemMetrics(SM_CXPADDEDBORDER));
    const int border_width = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));

    QScreen *screen = windowHandle()->screen();
    QRect rect;

    if (!isMinimized() && isVisible())
        rect = frameGeometry();
    else
        rect = screen->availableGeometry();

    QRect screen_rect = screen->availableGeometry();

    QRect dialog_rect = widget->geometry();

    dialog_rect.moveCenter(rect.center());

    dialog_rect.moveLeft(qMax(dialog_rect.left(), screen_rect.left() + border_width));
    dialog_rect.moveTop(qMax(dialog_rect.top() + titleBarHeight() / 2, screen_rect.top() + title_bar_height));
    dialog_rect.moveRight(qMin(dialog_rect.right(), screen_rect.right() - border_width));
    dialog_rect.moveBottom(qMin(dialog_rect.bottom(), screen_rect.bottom() - border_width));

    widget->setGeometry(dialog_rect);

    if (widget->windowIcon().isNull())
        widget->setWindowIcon(windowIcon());

    if (isMinimized())
        showNormal();
}

bool QGoodWindow::winButtonHover(long button)
{
    if (button == -1)
        return false;

    switch (button)
    {
    case HTMINBUTTON:
    case HTMAXBUTTON:
    case HTCLOSE:
    {
        if (!m_is_caption_button_pressed)
        {
            if (button != m_last_caption_button_hovered)
                buttonLeave(m_last_caption_button_hovered);

            if (isEnabled())
                buttonEnter(button);
        }
        else
        {
            if (button != m_last_caption_button_hovered)
                buttonLeave(m_last_caption_button_hovered);

            if (!m_is_caption_button_pressed || (button == m_caption_button_pressed))
                buttonEnter(button);
        }

        return true;
    }
    default:
    {
        buttonLeave(m_last_caption_button_hovered);
        return false;
    }
    }
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
    case HTMINBUTTON:
    case HTMAXBUTTON:
    case HTCLOSE:
    {
        if (QApplication::overrideCursor() &&
                QApplication::overrideCursor()->shape() == Qt::ArrowCursor)
            break;

        QApplication::setOverrideCursor(Qt::ArrowCursor);

        break;
    }
    case NO_WHERE:
    {
        if (!QApplication::overrideCursor())
            break;

        if (QApplication::overrideCursor()->shape() != Qt::ArrowCursor)
            break;

        QApplication::restoreOverrideCursor();

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
        return;

    const int border_width = qCeil(BORDERWIDTH * m_pixel_ratio);

    QRect geom = geometry();
    geom.adjust(-border_width, -border_width, border_width, border_width);

    QRegion available_geom;

    for (const QScreen *screen : QApplication::screens())
    {
        available_geom += screen->availableGeometry();
    }

    {   //TOP LEFT
        Shadow *shadow = m_shadow_list.at(0);

        QRect geom1 = geom;
        geom1.moveTo(geom1.left(), geom1.top());
        geom1.setWidth(border_width);
        geom1.setHeight(border_width);

        if (available_geom.intersects(geom1))
        {
            shadow->setGeometry(geom1);
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()).boundingRect());
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
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()).boundingRect());
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
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()).boundingRect());
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
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()).boundingRect());
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
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()).boundingRect());
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
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()).boundingRect());
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
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()).boundingRect());
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
            shadow->setGeometry(available_geom.intersected(shadow->frameGeometry()).boundingRect());
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
    {
        m_on_animate_event = true;
        macOSNative::setStyle(long(winId()), true);
    }
    else if (notification == "NSWindowDidExitFullScreenNotification")
    {
        macOSNative::setStyle(long(winId()), false);
        m_on_animate_event = false;
    }
    else if (notification == "AppleInterfaceThemeChangedNotification")
    {
        if (m_theme_change_timer)
            m_theme_change_timer->start();
    }
}
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
int QGoodWindow::ncHitTest(int x, int y)
{
    const int title_height = titleBarHeight(); //in pixels.

    //Get the point coordinates for the hit test.
    QPoint mouse_pos = QPoint(x, y);

    //Get the window rectangle.
    QRect window_rect = frameGeometry();

    //Determine if the hit test is for resizing. Default middle (1,1).
    int row = 1;
    int col = 1;
    bool on_resize_border = false;

    //Determine if the point is at the top or bottom of the window.
    if (mouse_pos.y() < window_rect.top() + title_height)
    {
        on_resize_border = (mouse_pos.y() < window_rect.top());
        row = 0;
    }
    else if (mouse_pos.y() > window_rect.bottom())
    {
        row = 2;
    }

    //Determine if the point is at the left or right of the window.
    if (mouse_pos.x() < window_rect.left())
    {
        col = 0; //left side
    }
    else if (mouse_pos.x() > window_rect.right())
    {
        col = 2; //right side
    }
    else if (row == 0 && !on_resize_border)
    {
        const QRegion left_mask = m_left_mask.translated(iconWidth(), 0);
        const QRegion right_mask = m_right_mask.translated(width() - rightMargin(), 0);

        const QPoint mouse_pos_map = mapFromGlobal(mouse_pos);

        if (mouse_pos.x() > window_rect.right() - rightMargin())
        {
            if (m_caption_buttons_handled && m_caption_buttons_corner == Qt::TopRightCorner)
            {
                QPoint pos = mouse_pos_map;
                pos.setX(pos.x() - (window_rect.right() - window_rect.left() - rightMargin()));

                if (m_cls_mask.contains(pos))
                    return HTCLOSE; //title bar close button
                else if (m_max_mask.contains(pos))
                    return HTMAXBUTTON; //title bar maximize button
                else if (m_min_mask.contains(pos))
                    return HTMINBUTTON; //title bar minimize button
            }

            if (right_mask.contains(mouse_pos_map))
                return NO_WHERE; //custom title bar buttons right
        }
        else if (mouse_pos.x() > window_rect.left() + iconWidth() &&
                 mouse_pos.x() < window_rect.left() + iconWidth() + leftMargin())
        {
            if (m_caption_buttons_handled && m_caption_buttons_corner == Qt::TopLeftCorner)
            {
                QPoint pos = mouse_pos_map;
                pos.setX(pos.x() - iconWidth());

                if (m_cls_mask.contains(pos))
                    return HTCLOSE; //title bar close button
                else if (m_max_mask.contains(pos))
                    return HTMAXBUTTON; //title bar maximize button
                else if (m_min_mask.contains(pos))
                    return HTMINBUTTON; //title bar minimize button
            }

            if (left_mask.contains(mouse_pos_map))
                return NO_WHERE; //custom title bar buttons left
        }
    }

    //Hit test (TOP_LEFT, ... BOTTOM_RIGHT)
    int hitTests[3][3] =
    {
        {TOP_LEFT, on_resize_border ? TOP : TITLE_BAR, TOP_RIGHT},
        {LEFT, NO_WHERE, RIGHT},
        {BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT},
    };

    return hitTests[row][col];
}
#endif
#ifdef QGOODWINDOW
void QGoodWindow::buttonEnter(long button)
{
    if (button == -1)
        return;

    m_last_caption_button_hovered = button;

    switch (button)
    {
    case HTMINBUTTON:
    {
        emit captionButtonStateChanged(CaptionButtonState::MinimizeHoverEnter);

        break;
    }
    case HTMAXBUTTON:
    {
        emit captionButtonStateChanged(CaptionButtonState::MaximizeHoverEnter);

        break;
    }
    case HTCLOSE:
    {
        emit captionButtonStateChanged(CaptionButtonState::CloseHoverEnter);

        break;
    }
    default:
        break;
    }
}

void QGoodWindow::buttonLeave(long button)
{
    if (button == -1)
        return;

    if (button != m_last_caption_button_hovered)
        return;

    m_last_caption_button_hovered = -1;

    switch (button)
    {
    case HTMINBUTTON:
    {
        emit captionButtonStateChanged(CaptionButtonState::MinimizeHoverLeave);

        break;
    }
    case HTMAXBUTTON:
    {
        emit captionButtonStateChanged(CaptionButtonState::MaximizeHoverLeave);

        break;
    }
    case HTCLOSE:
    {
        emit captionButtonStateChanged(CaptionButtonState::CloseHoverLeave);

        break;
    }
    default:
        break;
    }
}

bool QGoodWindow::buttonPress(long button)
{
    if (button == -1)
        return false;

    switch (button)
    {
    case HTMINBUTTON:
    {
        m_is_caption_button_pressed = true;
        m_caption_button_pressed = HTMINBUTTON;

        emit captionButtonStateChanged(CaptionButtonState::MinimizePress);

        activateWindow();

        return true;
    }
    case HTMAXBUTTON:
    {
        m_is_caption_button_pressed = true;
        m_caption_button_pressed = HTMAXBUTTON;

        emit captionButtonStateChanged(CaptionButtonState::MaximizePress);

        activateWindow();

        return true;
    }
    case HTCLOSE:
    {
        m_is_caption_button_pressed = true;
        m_caption_button_pressed = HTCLOSE;

        emit captionButtonStateChanged(CaptionButtonState::ClosePress);

        activateWindow();

        return true;
    }
    default:
        break;
    }

    return false;
}

bool QGoodWindow::buttonRelease(long button, bool valid_click)
{
    if (button == -1)
        return false;

    if (!m_is_caption_button_pressed)
        return false;

    if (m_hover_timer->isActive())
        return false;

    m_is_caption_button_pressed = false;
    m_caption_button_pressed = -1;

    switch (button)
    {
    case HTMINBUTTON:
    {
        emit captionButtonStateChanged(CaptionButtonState::MinimizeRelease);

        if (valid_click)
        {
            emit captionButtonStateChanged(CaptionButtonState::MinimizeClicked);
            m_hover_timer->start();
        }

        return true;
    }
    case HTMAXBUTTON:
    {
        emit captionButtonStateChanged(CaptionButtonState::MaximizeRelease);

        if (valid_click)
        {
            emit captionButtonStateChanged(CaptionButtonState::MaximizeClicked);
            m_hover_timer->start();
        }

        return true;
    }
    case HTCLOSE:
    {
        emit captionButtonStateChanged(CaptionButtonState::CloseRelease);

        if (valid_click)
        {
            emit captionButtonStateChanged(CaptionButtonState::CloseClicked);
            m_hover_timer->start();
        }

        return true;
    }
    default:
        break;
    }

    return false;
}
#endif
