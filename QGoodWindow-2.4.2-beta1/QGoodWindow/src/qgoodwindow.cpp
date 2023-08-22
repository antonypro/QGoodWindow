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

#if defined QGOODWINDOW && defined __linux__
#include <gtk/gtk.h>
#endif

#include "common.h"
#include "qgoodwindow.h"
#include "shadow.h"
#include "qgooddialog.h"
#include "../version/version.h"

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

#ifdef QGOODWINDOW
#define FIXED_WIDTH(gw) (gw->minimumWidth() >= gw->maximumWidth())
#define FIXED_HEIGHT(gw) (gw->minimumHeight() >= gw->maximumHeight())
#define FIXED_SIZE(gw) (FIXED_WIDTH(gw) && FIXED_HEIGHT(gw))
#endif

#ifdef Q_OS_WIN

#ifdef QT_VERSION_QT5
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
            QWidget *parent_widget = parentWidget();

            if (parent_widget)
                parent_widget->activateWindow();

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

    bool nativeEventFilter(const QByteArray &eventType, void *message, qgoodintptr *result) override
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

            HRESULT lresult = HRESULT(m_gw->ncHitTest(pos.x(), pos.y()));

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

inline bool isWinXOrGreater(DWORD major_version, DWORD minor_version, DWORD build_number)
{
    bool is_win_x_or_greater = false;

    typedef NTSTATUS(WINAPI *tRtlGetVersion)(LPOSVERSIONINFOEXW);
    tRtlGetVersion pRtlGetVersion = tRtlGetVersion(QLibrary::resolve("ntdll", "RtlGetVersion"));

    if (pRtlGetVersion)
    {
        OSVERSIONINFOEXW os_info;
        memset(&os_info, 0, sizeof(OSVERSIONINFOEXW));
        os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
        NTSTATUS status = pRtlGetVersion(&os_info);
        if (status == 0)
        {
            is_win_x_or_greater = (os_info.dwMajorVersion >= major_version &&
                                   os_info.dwMinorVersion >= minor_version &&
                                   os_info.dwBuildNumber >= build_number);
        }
    }

    return is_win_x_or_greater;
}

inline bool isWin11OrGreater()
{
    bool is_win_11_or_greater = isWinXOrGreater(10, 0, 22000);

    return is_win_11_or_greater;
}
}
#endif

#ifdef Q_OS_LINUX

#include <QtTest/QtTest>
#ifdef QT_VERSION_QT5
#include <QtX11Extras/QX11Info>
#endif
#ifdef QT_VERSION_QT6
#include <QtGui/private/qtx11extras_p.h>
#endif
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <xcb/xcb.h>

namespace QGoodWindowUtils
{
QList<QGoodWindow*> m_gw_list;

GtkSettings *m_settings = nullptr;

void themeChangeNotification()
{
    for (QGoodWindow *gw : m_gw_list)
    {
        QTimer::singleShot(0, gw, &QGoodWindow::themeChanged);
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

#ifndef Q_OS_WIN
#define GOODPARENT(parent) parent
#else
#define GOODPARENT(parent) nullptr
#endif

QGoodWindow::QGoodWindow(QWidget *parent, const QColor &clear_color) : QMainWindow(GOODPARENT(parent))
{
#ifdef QGOODWINDOW
    qRegisterMetaType<QGoodWindow::CaptionButtonState>("QGoodWindow::CaptionButtonState");

    m_parent = parent;

    m_is_using_system_borders = shouldBordersBeDrawnBySystem();

    m_dark = isSystemThemeDark();

    m_title_bar_height = 30;
    m_icon_width = 0;

    m_is_caption_button_pressed = false;
    m_last_caption_button_hovered = -1;
    m_caption_button_pressed = -1;

    m_hover_timer = new QTimer(this);
    m_hover_timer->setSingleShot(true);
    m_hover_timer->setInterval(300);

    m_pixel_ratio = qreal(1);
#endif
#ifdef Q_OS_WIN
    m_clear_color = clear_color;
#else
    Q_UNUSED(clear_color)
#endif
#ifdef Q_OS_WIN
    m_minimum_width = 0;
    m_minimum_height = 0;
    m_maximum_width = 0;
    m_maximum_height = 0;
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
    m_last_move_button = -1;
#endif
#ifdef Q_OS_MAC
    m_mouse_button_pressed = false;
    m_on_animate_event = false;
#endif
#ifdef Q_OS_WIN
    m_window_ready_for_resize = false;
    m_closed = false;
    m_visible = false;
    m_self_generated_close_event = false;

    m_timer_move = new QTimer(this);
    connect(m_timer_move, &QTimer::timeout, this, [=]{
        QScreen *screen = screenForWindow(m_hwnd);

        if (windowHandle()->screen() != screen)
            updateScreen(screen);
    });
    m_timer_move->setSingleShot(true);
    m_timer_move->setInterval(0);

    m_window_state = Qt::WindowNoState;

    m_is_menu_visible = false;
    m_timer_menu = new QTimer(this);
    m_timer_menu->setSingleShot(true);
    m_timer_menu->setInterval(qApp->doubleClickInterval());

    m_active_state = false;

    m_last_state = Qt::WindowNoState;

    m_self_generated_show_event = false;

    m_self_generated_resize_event = false;

    m_native_event = nullptr;

    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    HWND parent_hwnd = nullptr;

    if (m_parent)
        parent_hwnd = HWND(m_parent->winId());

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
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           parent_hwnd, nullptr, hInstance, nullptr);

    if (!m_hwnd)
    {
        QMessageBox::critical(nullptr, "Error", "Error creating window");
        return;
    }

    SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    //Fix QApplication::exit() crash.
    connect(qApp, &QApplication::aboutToQuit, this, &QGoodWindow::closeGW);

    m_is_win_11_or_greater = QGoodWindowUtils::isWin11OrGreater();

    m_window_handle = QWindow::fromWinId(WId(m_hwnd));

    initGW();

    m_helper_widget = new QWidget();

    //Fix bug that prevents window to close on Qt 6.
#ifdef QT_VERSION_QT6
    m_helper_window = new QWindow();
    m_helper_window->setFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool);
    m_helper_window->setGeometry(0, 0, 1, 1);
    m_helper_window->setOpacity(0);
#endif
    if (!m_is_using_system_borders)
    {
        QGoodWindow *parent = m_parent ? this : nullptr;
        m_shadow = new Shadow(qintptr(m_hwnd), parent, parent);
        m_shadow->createWinId();
        connect(m_shadow, &Shadow::showSignal, this, &QGoodWindow::moveShadow);
    }

    m_main_window = static_cast<QMainWindow*>(this);
    m_main_window->createWinId();
    m_main_window->installEventFilter(this);

    QWidget *widget_proxy = new QWidget(m_main_window);
    m_main_window->setCentralWidget(widget_proxy);

    m_proxy_widget = new QWidget(widget_proxy);

    QScreen *screen = screenForWindow(m_hwnd);
    m_pixel_ratio = screen->devicePixelRatio();
    setCurrentScreen(screen);

    if (m_parent)
        m_parent->installEventFilter(this);

    if (!m_native_event)
    {
        m_native_event = new QGoodWindowUtils::NativeEventFilter(this);
        qApp->installNativeEventFilter(m_native_event);
    }

    //Fix that hides native title bar.
    frameChanged();

#ifdef QT_VERSION_QT5
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

    installEventFilter(this);
    setMouseTracking(true);

    createWinId();

    m_pixel_ratio = windowHandle()->screen()->devicePixelRatio();

    QGoodWindowUtils::registerThemeChangeNotification();

    QGoodWindowUtils::m_gw_list.append(this);

    //Fake window flags.
    m_window_flags = Qt::Window | Qt::FramelessWindowHint;

    if (!m_parent)
    {
        QMainWindow::setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        m_window_flags |= Qt::WindowMinMaxButtonsHint;
    }
    else
    {
        QMainWindow::setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool);
    }

    m_top_shadow = new Shadow(qintptr(nullptr), this, this);
    m_top_shadow->installEventFilter(this);
    m_top_shadow->setMouseTracking(true);
    connect(m_top_shadow, &Shadow::showSignal, this, &QGoodWindow::sizeMoveBorders);

    m_bottom_shadow = new Shadow(qintptr(nullptr), this, this);
    m_bottom_shadow->installEventFilter(this);
    m_bottom_shadow->setMouseTracking(true);
    connect(m_bottom_shadow, &Shadow::showSignal, this, &QGoodWindow::sizeMoveBorders);

    m_left_shadow = new Shadow(qintptr(nullptr), this, this);
    m_left_shadow->installEventFilter(this);
    m_left_shadow->setMouseTracking(true);
    connect(m_left_shadow, &Shadow::showSignal, this, &QGoodWindow::sizeMoveBorders);

    m_right_shadow = new Shadow(qintptr(nullptr), this, this);
    m_right_shadow->installEventFilter(this);
    m_right_shadow->setMouseTracking(true);
    connect(m_right_shadow, &Shadow::showSignal, this, &QGoodWindow::sizeMoveBorders);
#endif
#ifdef Q_OS_MAC
    installEventFilter(this);
    setMouseTracking(true);

    createWinId();

    notification.addWindow(this);

    if (!QGoodWindowUtils::m_theme_change_registered)
    {
        QGoodWindowUtils::m_theme_change_registered = true;
        macOSNative::registerThemeChangeNotification();
    }

    if (!m_parent)
        QMainWindow::setWindowFlags(Qt::Window);
    else
        QMainWindow::setWindowFlags(Qt::Dialog);
#endif
#ifdef QGOODWINDOW
    auto func_default_name_icon = [=]{
        if (windowTitle().isEmpty())
        {
            setWindowTitle(qApp->applicationName());
            QTimer::singleShot(0, this, [=]{Q_EMIT windowTitleChanged(windowTitle());});
        }

        if (windowIcon().isNull())
        {
            setWindowIcon(qApp->style()->standardIcon(QStyle::SP_DesktopIcon));
            QTimer::singleShot(0, this, [=]{Q_EMIT windowIconChanged(windowIcon());});
        }
    };
#ifdef Q_OS_WIN
    QTimer::singleShot(0, this, func_default_name_icon);
#else
    func_default_name_icon();
#endif
#endif
}

QGoodWindow::~QGoodWindow()
{
#ifdef Q_OS_WIN
    if (!m_closed)
    {
        //Last chance to close window.
        close();
    }

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

void QGoodWindow::themeChanged()
{
#ifdef QGOODWINDOW
    bool dark = isSystemThemeDark();

    if (m_dark != dark)
    {
        m_dark = dark;
        Q_EMIT systemThemeChanged();
    }
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

void QGoodWindow::setWindowFlags(Qt::WindowFlags type)
{
#ifdef QGOODWINDOW
    Q_UNUSED(type)
#else
    QMainWindow::setWindowFlags(type);
#endif
}

Qt::WindowFlags QGoodWindow::windowFlags() const
{
#if defined Q_OS_WIN
    HWND hwnd = HWND(winId());

    Qt::WindowFlags flags;
    flags |= Qt::Window;
    flags |= Qt::WindowTitleHint;
    flags |= Qt::WindowSystemMenuHint;

    if (GetWindowLongW(hwnd, GWL_STYLE) & WS_MINIMIZEBOX)
        flags |= Qt::WindowMinimizeButtonHint;

    if (GetWindowLongW(hwnd, GWL_STYLE) & WS_MAXIMIZEBOX)
        flags |= Qt::WindowMaximizeButtonHint;

    flags |= Qt::WindowCloseButtonHint;

    return flags;
#elif defined Q_OS_LINUX
    return m_window_flags;
#else
    return QMainWindow::windowFlags();
#endif
}

/*** QGOODWINDOW FUNCTIONS BEGIN ***/

QString QGoodWindow::version()
{
    return QStringLiteral(QGOODWINDOW_VERSION);
}

void QGoodWindow::setup()
{
#ifdef QGOODWINDOW
    //Init resources
    Q_INIT_RESOURCE(qgoodwindow_style);
#ifdef QGOODCENTRALWIDGET
    Q_INIT_RESOURCE(qgoodcentralwidget_icons);
#endif

#ifdef Q_OS_LINUX
    qputenv("XDG_SESSION_TYPE", "xcb");
    qputenv("QT_QPA_PLATFORM", "xcb");

    int argc = 0;
    char **argv = nullptr;
    gtk_init(&argc, &argv);
#endif

#ifndef Q_OS_MAC

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    if (QGoodWindowUtils::isWinXOrGreater(10, 0, 15063)) //Windows 10 version 1703 or later.
    {
        static const qintptr vDPI_AWARENESS_CONTEXT = 0;
        static const qintptr vDPI_AWARENESS_CONTEXT_UNAWARE = ((vDPI_AWARENESS_CONTEXT)-1);
        static const qintptr vDPI_AWARENESS_CONTEXT_SYSTEM_AWARE = ((vDPI_AWARENESS_CONTEXT)-2);
        static const qintptr vDPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE = ((vDPI_AWARENESS_CONTEXT)-3);
        static const qintptr vDPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 = ((vDPI_AWARENESS_CONTEXT)-4);
        static const qintptr vDPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED = ((vDPI_AWARENESS_CONTEXT)-5);

        Q_UNUSED(vDPI_AWARENESS_CONTEXT_UNAWARE)
        Q_UNUSED(vDPI_AWARENESS_CONTEXT_SYSTEM_AWARE)
        Q_UNUSED(vDPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)
        Q_UNUSED(vDPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
        Q_UNUSED(vDPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED)

        typedef BOOL(*tSetProcessDpiAwarenessContext)(HANDLE);
        tSetProcessDpiAwarenessContext pSetProcessDpiAwarenessContext =
                tSetProcessDpiAwarenessContext(QLibrary::resolve("user32", "SetProcessDpiAwarenessContext"));

        if (pSetProcessDpiAwarenessContext)
            pSetProcessDpiAwarenessContext(HANDLE(vDPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2));
    }
#endif
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
#elif QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    qputenv("QT_DEVICE_PIXEL_RATIO", "auto");
#endif

#endif

#ifdef Q_OS_MAC
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
#endif
#endif
}

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

int QGoodWindow::execDialog(QDialog *dialog, QGoodWindow *child_gw, QGoodWindow *parent_gw)
{
#ifdef QGOODWINDOW
    return QGoodDialog::exec(dialog, child_gw, parent_gw);
#else
    Q_UNUSED(child_gw)
    Q_UNUSED(parent_gw)
    return dialog->exec();
#endif
}

void QGoodWindow::setAppDarkTheme()
{
    qApp->setStyle(new DarkStyle());
    qApp->style()->setObjectName("fusion");
}

void QGoodWindow::setAppLightTheme()
{
    qApp->setStyle(new LightStyle());
    qApp->style()->setObjectName("fusion");
}

QGoodStateHolder *QGoodWindow::qGoodStateHolderInstance()
{
    return QGoodStateHolder::instance();
}

void QGoodWindow::setTitleBarHeight(int height)
{
#ifdef QGOODWINDOW
    m_title_bar_height = height;
#else
    Q_UNUSED(height)
#endif
}

void QGoodWindow::setIconWidth(int width)
{
#ifdef QGOODWINDOW
    m_icon_width = width;
#else
    Q_UNUSED(width)
#endif
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

QRect QGoodWindow::titleBarRect() const
{
#ifdef QGOODWINDOW
    if (!m_is_using_system_borders)
    {
        if (windowState().testFlag(Qt::WindowNoState))
        {
            return QRect(1, 1, width() - 2, titleBarHeight());
        }
        else
        {
            return QRect(0, 0, width(), titleBarHeight());
        }
    }
    else
    {
        return QRect(0, 0, width(), titleBarHeight());
    }
#else
    return QRect();
#endif
}

void QGoodWindow::setTitleBarMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    m_title_bar_mask = mask;
#else
    Q_UNUSED(mask)
#endif
}

void QGoodWindow::setMinimizeMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    m_min_mask = mask;
#else
    Q_UNUSED(mask)
#endif
}

void QGoodWindow::setMaximizeMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    m_max_mask = mask;
#else
    Q_UNUSED(mask)
#endif
}

void QGoodWindow::setCloseMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    m_cls_mask = mask;
#else
    Q_UNUSED(mask)
#endif
}

QRegion QGoodWindow::titleBarMask() const
{
#ifdef QGOODWINDOW
    return m_title_bar_mask;
#else
    return QRegion();
#endif
}

QRegion QGoodWindow::minimizeMask() const
{
#ifdef QGOODWINDOW
    return m_min_mask;
#else
    return QRegion();
#endif
}

QRegion QGoodWindow::maximizeMask() const
{
#ifdef QGOODWINDOW
    return m_max_mask;
#else
    return QRegion();
#endif
}

QRegion QGoodWindow::closeMask() const
{
#ifdef QGOODWINDOW
    return m_cls_mask;
#else
    return QRegion();
#endif
}
/*** QGOODWINDOW FUNCTIONS END ***/

void QGoodWindow::setCentralWidget(QWidget *widget)
{
#ifdef Q_OS_WIN
    if (m_proxy_widget->layout())
        delete m_proxy_widget->layout();

    QHBoxLayout *layout = new QHBoxLayout(m_proxy_widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(widget);

    m_central_widget = widget;
#else
    QMainWindow::setCentralWidget(widget);
#endif
}

QWidget *QGoodWindow::centralWidget() const
{
#ifdef Q_OS_WIN
    return m_central_widget;
#else
    return QMainWindow::centralWidget();
#endif
}

QSize QGoodWindow::sizeHint() const
{
#ifdef Q_OS_WIN
    if (!m_central_widget)
        return QMainWindow::sizeHint();

    return m_central_widget->sizeHint();
#else
    return QMainWindow::sizeHint();
#endif
}

QSize QGoodWindow::minimumSizeHint() const
{
#ifdef Q_OS_WIN
    if (!m_central_widget)
        return QMainWindow::minimumSizeHint();

    return m_central_widget->minimumSizeHint();
#else
    return QMainWindow::minimumSizeHint();
#endif
}

void QGoodWindow::setFixedSize(int w, int h)
{
#ifdef Q_OS_WIN
    bool already_fixed = FIXED_SIZE(this);

    setMinimumWidth(w);
    setMaximumWidth(w);

    setMinimumHeight(h);
    setMaximumHeight(h);

    if (!already_fixed)
    {
        SetWindowLongW(m_hwnd, GWL_STYLE,
                       WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
                           (!m_parent ? WS_MINIMIZEBOX : 0));
    }

    resize(w, h);
#else
#ifdef Q_OS_LINUX
    m_window_flags ^= Qt::WindowMaximizeButtonHint;
#endif
    QMainWindow::setFixedSize(w, h);
#endif
}

void QGoodWindow::setFixedSize(const QSize &size)
{
#if defined Q_OS_WIN
    setFixedSize(size.width(), size.height());
#else
    QMainWindow::setFixedSize(size);
#endif
}

void QGoodWindow::setMinimumSize(const QSize &size)
{
#ifdef Q_OS_WIN
    setMinimumWidth(size.width());
    setMinimumHeight(size.height());
#else
    QMainWindow::setMinimumSize(size);
#endif
}

void QGoodWindow::setMaximumSize(const QSize &size)
{
#ifdef Q_OS_WIN
    setMaximumWidth(size.width());
    setMaximumHeight(size.height());
#else
    QMainWindow::setMaximumSize(size);
#endif
}

void QGoodWindow::setMinimumWidth(int w)
{
#ifdef Q_OS_WIN
    m_minimum_width = qMax(w, 0);
#else
    QMainWindow::setMinimumWidth(w);
#endif
}

void QGoodWindow::setMinimumHeight(int h)
{
#ifdef Q_OS_WIN
    m_minimum_height = qMax(h, 0);
#else
    QMainWindow::setMinimumHeight(h);
#endif
}

void QGoodWindow::setMaximumWidth(int w)
{
#ifdef Q_OS_WIN
    m_maximum_width = qMax(w, 0);
#else
    QMainWindow::setMaximumWidth(w);
#endif
}

void QGoodWindow::setMaximumHeight(int h)
{
#ifdef Q_OS_WIN
    m_maximum_height = qMax(h, 0);
#else
    QMainWindow::setMaximumHeight(h);
#endif
}

QSize QGoodWindow::minimumSize() const
{
#ifdef Q_OS_WIN
    return QSize(minimumWidth(), minimumHeight());
#else
    return QMainWindow::minimumSize();
#endif
}

QSize QGoodWindow::maximumSize() const
{
#ifdef Q_OS_WIN
    return QSize(maximumWidth(), maximumHeight());
#else
    return QMainWindow::maximumSize();
#endif
}

int QGoodWindow::minimumWidth() const
{
#ifdef Q_OS_WIN
    return (m_minimum_width > 0 ? m_minimum_width : QMainWindow::minimumWidth());
#else
    return QMainWindow::minimumWidth();
#endif
}

int QGoodWindow::minimumHeight() const
{
#ifdef Q_OS_WIN
    return (m_minimum_height > 0 ? m_minimum_height : QMainWindow::minimumHeight());
#else
    return QMainWindow::minimumHeight();
#endif
}

int QGoodWindow::maximumWidth() const
{
#ifdef Q_OS_WIN
    return (m_maximum_width > 0 ? m_maximum_width : QMainWindow::maximumWidth());
#else
    return QMainWindow::maximumWidth();
#endif
}

int QGoodWindow::maximumHeight() const
{
#ifdef Q_OS_WIN
    return (m_maximum_height > 0 ? m_maximum_height : QMainWindow::maximumHeight());
#else
    return QMainWindow::maximumHeight();
#endif
}

QRect QGoodWindow::normalGeometry() const
{
#ifdef Q_OS_WIN
    return m_rect_normal;
#else
    return QMainWindow::normalGeometry();
#endif
}

QRect QGoodWindow::frameGeometry() const
{
#ifdef Q_OS_WIN
    RECT window_rect;
    GetWindowRect(m_hwnd, &window_rect);

    int x = qFloor(window_rect.left / m_pixel_ratio);
    int y = qFloor(window_rect.top / m_pixel_ratio);
    int w = qFloor((window_rect.right - window_rect.left) / m_pixel_ratio);
    int h = qFloor((window_rect.bottom - window_rect.top) / m_pixel_ratio);

    QRect rect = QRect(x, y, w, h);

    if (isMaximized())
    {
        const int border_width = BORDERWIDTH();
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

    int w = qFloor((client_rect.right - client_rect.left) / m_pixel_ratio);
    int h = qFloor((client_rect.bottom - client_rect.top) / m_pixel_ratio);

    QRect rect = QRect(x(), y(), w, h);

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
    x = qFloor(x * m_pixel_ratio);
    y = qFloor(y * m_pixel_ratio);

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
    width = qFloor(width * m_pixel_ratio);
    height = qFloor(height * m_pixel_ratio);

    if (m_is_using_system_borders)
    {
        width += BORDERWIDTH() * 2;
        height += BORDERHEIGHT();
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
    if (!isVisible() && !m_window_state.testFlag(Qt::WindowNoState))
    {
        setWindowStateWin();
        return;
    }

    ShowWindow(m_hwnd, SW_SHOW);
#else
    QMainWindow::show();
#endif
}

void QGoodWindow::showNormal()
{
#ifdef Q_OS_WIN
    if (isFullScreen())
    {
        m_window_state ^= Qt::WindowFullScreen;
        SetWindowLongW(m_hwnd, GWL_STYLE, GetWindowLongW(m_hwnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);
        setGeometry(normalGeometry());
        setWindowMask();
    }

    ShowWindow(m_hwnd, SW_SHOWNORMAL);
#else
    QMainWindow::showNormal();
#endif
}

void QGoodWindow::showMaximized()
{
#if defined Q_OS_WIN || defined Q_OS_LINUX
    if (FIXED_SIZE(this))
        return;
#endif
#ifdef Q_OS_WIN
    if (isFullScreen())
        showNormal();

    ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
#else
    QMainWindow::showMaximized();
#endif
}

void QGoodWindow::showMinimized()
{
#ifdef Q_OS_WIN
    if (isFullScreen())
        showNormal();

    ShowWindow(m_hwnd, SW_MINIMIZE);
#else
    QMainWindow::showMinimized();
#endif
}

void QGoodWindow::showFullScreen()
{
#if defined Q_OS_WIN || defined Q_OS_LINUX
    if (FIXED_SIZE(this))
        return;
#endif
#ifdef Q_OS_WIN
    if (isFullScreen())
        return;

    m_window_state = Qt::WindowFullScreen;

    ShowWindow(m_hwnd, SW_SHOWNORMAL);

    SetWindowLongW(m_hwnd, GWL_STYLE, GetWindowLongW(m_hwnd, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);

    HMONITOR monitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitor_info;
    memset(&monitor_info, 0, sizeof(MONITORINFO));
    monitor_info.cbSize = sizeof(MONITORINFO);

    GetMonitorInfoW(monitor, &monitor_info);

    QRect screen_geom;
    screen_geom.moveTop(monitor_info.rcMonitor.top);
    screen_geom.moveLeft(monitor_info.rcMonitor.left);
    screen_geom.setWidth(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left);
    screen_geom.setHeight(monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top);

    SetWindowPos(m_hwnd, nullptr, screen_geom.x(), screen_geom.y(),
                 screen_geom.width(), screen_geom.height(), SWP_NOACTIVATE);

    QRect window_rect;
    window_rect.moveTop(0);
    window_rect.moveLeft(0);
    window_rect.setWidth(screen_geom.width());
    window_rect.setHeight(screen_geom.height());

#ifdef QT_VERSION_QT5
    HRGN hrgn = QtWin::toHRGN(QRegion(window_rect));
#endif
#ifdef QT_VERSION_QT6
    HRGN hrgn = QRegion(window_rect).toHRGN();
#endif

    SetWindowRgn(m_hwnd, hrgn, TRUE);

    sizeMoveWindow();
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

bool QGoodWindow::close()
{
#ifdef Q_OS_WIN
    if (m_closed)
        return true;

    SendMessageW(m_hwnd, WM_CLOSE, 0, 0);

    return m_closed;
#else
    return QMainWindow::close();
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
    const HWND active_window = GetActiveWindow();
    return ((active_window == m_hwnd) || IsChild(m_hwnd, active_window));
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
    return windowState().testFlag(Qt::WindowFullScreen);
#else
    return QMainWindow::isFullScreen();
#endif
}

Qt::WindowStates QGoodWindow::windowState() const
{
#ifdef Q_OS_WIN
    return m_window_state;
#else
    return QMainWindow::windowState();
#endif
}

void QGoodWindow::setWindowState(Qt::WindowStates state)
{
#ifdef Q_OS_WIN
    m_window_state = state;

    if (!isVisible())
        return;

    setWindowStateWin();
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

#ifdef QT_VERSION_QT5
    HICON hicon_big = QtWin::toHICON(icon.pixmap(GetSystemMetrics(SM_CXICON),
                                                 GetSystemMetrics(SM_CYICON)));
    HICON hicon_small = QtWin::toHICON(icon.pixmap(GetSystemMetrics(SM_CXSMICON),
                                                   GetSystemMetrics(SM_CYSMICON)));
#endif
#ifdef QT_VERSION_QT6
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

QByteArray QGoodWindow::saveGeometry() const
{
#ifdef QGOODWINDOW
    QByteArray geometry;
    QDataStream stream(&geometry, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);

    QRect window_geom = normalGeometry();
    window_geom.moveTop(qFloor(window_geom.y() * m_pixel_ratio));
    window_geom.moveLeft(qFloor(window_geom.x() * m_pixel_ratio));
    window_geom.setWidth(qFloor(window_geom.width() * m_pixel_ratio));
    window_geom.setHeight(qFloor(window_geom.height() * m_pixel_ratio));

    stream << qint32(window_geom.x());
    stream << qint32(window_geom.y());
    stream << qint32(window_geom.width());
    stream << qint32(window_geom.height());
    stream << windowState().testFlag(Qt::WindowMinimized);
    stream << windowState().testFlag(Qt::WindowMaximized);
    stream << windowState().testFlag(Qt::WindowFullScreen);

    return geometry;
#else
    return QMainWindow::saveGeometry();
#endif
}

bool QGoodWindow::restoreGeometry(const QByteArray &geometry)
{
#ifdef QGOODWINDOW
    if (geometry.size() != int(sizeof(qint32) * 4 + sizeof(bool) * 3))
        return false;

    QDataStream stream(geometry);
    stream.setVersion(QDataStream::Qt_5_0);

    qint32 rect_x;
    qint32 rect_y;
    qint32 rect_width;
    qint32 rect_height;
    bool minimized;
    bool maximized;
    bool fullscreen;

    stream >> rect_x;
    stream >> rect_y;
    stream >> rect_width;
    stream >> rect_height;
    stream >> minimized;
    stream >> maximized;
    stream >> fullscreen;

    if (stream.status() != QDataStream::Ok)
        return false;

    QRegion screens;

    for (QScreen *screen : qApp->screens())
    {
        QRect rect = screen->geometry();
        qreal pixel_ratio = screen->devicePixelRatio();
        rect.setWidth(qFloor(rect.width() * pixel_ratio));
        rect.setHeight(qFloor(rect.height() * pixel_ratio));
        screens += rect;
    }

    QRect window_geom;
    window_geom.moveTop(rect_y);
    window_geom.moveLeft(rect_x);
    window_geom.setWidth(rect_width);
    window_geom.setHeight(rect_height);

    if (!screens.boundingRect().contains(window_geom))
    {
        QScreen *screen = qApp->primaryScreen();

        QRect rect = screen->availableGeometry();
        qreal pixel_ratio = screen->devicePixelRatio();
        rect.setWidth(qFloor(rect.width() * pixel_ratio));
        rect.setHeight(qFloor(rect.height() * pixel_ratio));

        window_geom.moveCenter(rect.center());
        window_geom.setWidth(qMin(rect.width(), window_geom.width()));
        window_geom.setHeight(qMin(rect.height(), window_geom.height()));
    }

#ifdef Q_OS_WIN
    updateScreen(screenForPoint(window_geom.center()));
#endif

    window_geom.moveTop(qFloor(window_geom.y() / m_pixel_ratio));
    window_geom.moveLeft(qFloor(window_geom.x() / m_pixel_ratio));
    window_geom.setWidth(qFloor(window_geom.width() / m_pixel_ratio));
    window_geom.setHeight(qFloor(window_geom.height() / m_pixel_ratio));

#ifdef Q_OS_WIN
    m_rect_normal = window_geom;
#endif

    setGeometry(window_geom);

    if (minimized)
        setWindowState(Qt::WindowMinimized);
    else if (maximized)
        setWindowState(Qt::WindowMaximized);
    else if (fullscreen)
        setWindowState(Qt::WindowFullScreen);

    return true;
#else
    return QMainWindow::restoreGeometry(geometry);
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

        if (!widget)
            break;

//Catch QMenu show event to fix show bug.
#if defined QT_VERSION_QT5 && QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        for (QWidget *widget : widget->findChildren<QWidget*>())
        {
            if (widget->metaObject()->className() == QStringLiteral("QMenu") ||
                widget->metaObject()->className() == QStringLiteral("QComboBoxPrivateContainer"))
            {
                widget->installEventFilter(this);
            }
        }
#endif

        if (!widget->isWindow())
            break;

        if (!widget->isModal())
            break;

        bool is_file_dialog = bool(qobject_cast<QFileDialog*>(widget));

        widget->adjustSize();
        widget->installEventFilter(this);
        widget->setParent(bestParentForModalWindow(),
                          (!is_file_dialog ? widget->windowFlags() : Qt::WindowFlags(0)));

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
    case QEvent::Show:
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
        internalWidgetResize();

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
    case QEvent::Show:
    case QEvent::Hide:
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
    case QEvent::WindowStateChange:
    case QEvent::Resize:
    case QEvent::Move:
    {
        setMask((isVisible() && windowState().testFlag(Qt::WindowNoState)) ? rect() : QRect());

        if (isVisible() && windowState().testFlag(Qt::WindowNoState))
        {
            sizeMoveBorders();

            if (isActiveWindow())
            {
                m_top_shadow->showLater();
                m_bottom_shadow->showLater();
                m_left_shadow->showLater();
                m_right_shadow->showLater();
            }
        }
        else
        {
            m_top_shadow->hide();
            m_bottom_shadow->hide();
            m_left_shadow->hide();
            m_right_shadow->hide();
        }

        break;
    }
    case QEvent::WindowBlocked:
    {
        if (FIXED_SIZE(this))
            break;

        if (!windowState().testFlag(Qt::WindowNoState))
            break;

        m_top_shadow->hide();
        m_bottom_shadow->hide();
        m_left_shadow->hide();
        m_right_shadow->hide();

        break;
    }
    case QEvent::WindowUnblocked:
    {
        if (FIXED_SIZE(this))
            break;

        activateWindow();

        if (!windowState().testFlag(Qt::WindowNoState))
            break;

        m_top_shadow->show();
        m_bottom_shadow->show();
        m_left_shadow->show();
        m_right_shadow->show();

        break;
    }
    default:
        break;
    }
#endif
#ifdef Q_OS_MAC
    switch (event->type())
    {
    case QEvent::WinIdChange:
    {
        QTimer::singleShot(0, this, [=]{
            notification.unregisterNotification();

            notification.registerNotification("NSWindowWillEnterFullScreenNotification", winId());
            notification.registerNotification("NSWindowDidExitFullScreenNotification", winId());
        });

        break;
    }
    case QEvent::Show:
    {
        if (isFullScreen())
            break;

        macOSNative::setStyle(long(winId()), macOSNative::StyleType::NoState);

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
        case QEvent::Show:
        {
            //Prevent send multiple show events at the window show event.
            if (!m_self_generated_show_event)
                return true;

            //Don't propagate show event if already visible.
            if (m_visible)
                return true;

            m_visible = true;

            break;
        }
        case QEvent::Hide:
        {
            //Don't propagate hide event if already invisible.
            if (!m_visible)
                return true;

            m_visible = false;

            break;
        }
        case QEvent::WindowStateChange:
        {
            if (isMinimized())
                m_visible = false;

            break;
        }
        case QEvent::Close:
        {
            //Don't propagate close event if it is generated
            //by QGoodWindow.
            if (!m_self_generated_close_event)
                return true;

            break;
        }
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
    else if (m_parent && (watched == m_parent))
    {
        switch (event->type())
        {
        case QEvent::Close:
        {
            if (event->isAccepted())
                close();

            break;
        }
        default:
            break;
        }
    }
#if defined QT_VERSION_QT5 && QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    else if (watched->metaObject()->className() == QStringLiteral("QMenu") ||
               watched->metaObject()->className() == QStringLiteral("QComboBoxPrivateContainer"))
    {
        QWidget *widget = qobject_cast<QWidget*>(watched);

        if (widget)
        {
            switch (event->type())
            {
            case QEvent::Polish:
            {
                if (!widget->windowHandle())
                    widget->createWinId();

                break;
            }
            case QEvent::Show:
            {
                if (qobject_cast<QMenu*>(widget->parentWidget()))
                    break;

                QScreen *screen = windowHandle()->screen();
                widget->windowHandle()->setScreen(screen);

                int x = qFloor(screen->geometry().x() / m_pixel_ratio);
                int y = qFloor(screen->geometry().y() / m_pixel_ratio);

                x += widget->x();
                y += widget->y();

                widget->move(x, y);

                break;
            }
            default:
                break;
            }
        }
    }
#endif
    else if (QWidget *widget = qobject_cast<QWidget*>(watched))
    {
        if (widget->isWindow())
        {
            switch (event->type())
            {
            case QEvent::Show:
            {
                if (isMinimized())
                    showNormal();

                moveCenterWindow(widget);

                break;
            }
            case QEvent::Resize:
            {
                QTimer::singleShot(0, this, [=]{
                    moveCenterWindow(widget);
                });

                break;
            }
            default:
                break;
            }
        }
    }
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
    QWidget *modal_widget = qApp->activeModalWidget();

    if (modal_widget && modal_widget->window() != this)
        return QMainWindow::eventFilter(watched, event);

    QWidget *widget = qobject_cast<QWidget*>(watched);

    if (!widget)
        return QMainWindow::eventFilter(watched, event);

    if (widget->window() != this
        #ifdef Q_OS_LINUX
            && widget != m_top_shadow
            && widget != m_bottom_shadow
            && widget != m_left_shadow
            && widget != m_right_shadow
        #endif
            )
        return QMainWindow::eventFilter(watched, event);

    QPoint cursor_pos = QCursor::pos();
    qintptr button = ncHitTest(qFloor(cursor_pos.x() * m_pixel_ratio), qFloor(cursor_pos.y() * m_pixel_ratio));

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

        macOSNative::setStyle(long(winId()), macOSNative::StyleType::NoState);

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
            if (m_margin == HTCAPTION)
            {
                if (!isMaximized())
                    showMaximized();
                else
                    showNormal();
            }
        }

        switch (m_margin)
        {
        case HTCAPTION:
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
            if (m_margin != HTNOWHERE)
                m_resize_move = true;
        }

        switch (m_margin)
        {
        case HTCAPTION:
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
        case HTCAPTION:
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
            if (m_margin != HTNOWHERE)
                m_resize_move = false;
        }

        switch (m_margin)
        {
        case HTCAPTION:
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
        case HTCAPTION:
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
        case HTCAPTION:
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
    if (m_parent)
        return QMainWindow::eventFilter(watched, event);

    if (m_is_caption_button_pressed)
        return QMainWindow::eventFilter(watched, event);

    int margin = int(ncHitTest(qFloor(cursor_pos.x() * m_pixel_ratio), qFloor(cursor_pos.y() * m_pixel_ratio)));

    switch (event->type())
    {
    case QEvent::MouseButtonDblClick:
    {
        if (margin == HTCAPTION)
        {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

            if (!isFullScreen())
            {
                if (mouse_event->button() == Qt::LeftButton)
                {
                    if (margin == HTCAPTION)
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
        case HTCAPTION:
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
        if (margin == HTCAPTION)
        {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

            if (!isFullScreen())
            {
                if (mouse_event->button() == Qt::LeftButton)
                {
                    m_cursor_move_pos = cursor_pos - pos();
                }
            }
        }

        m_mouse_button_pressed = true;

        switch (margin)
        {
        case HTCAPTION:
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
        if (!m_cursor_move_pos.isNull())
        {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

            if (!isFullScreen())
            {
                if (mouse_event->buttons() == Qt::LeftButton)
                {
                    move(cursor_pos - m_cursor_move_pos);
                }
            }
        }

        if (!m_mouse_button_pressed)
        {
            QWidget *widget = QApplication::widgetAt(cursor_pos);

            if (widget)
            {
                switch (margin)
                {
                case HTCAPTION:
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
                case HTNOWHERE:
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
        case HTCAPTION:
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
        if (!m_cursor_move_pos.isNull())
        {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

            QWidget *widget = QApplication::widgetAt(cursor_pos);

            if (widget && !isFullScreen())
            {
                if (mouse_event->button() == Qt::LeftButton)
                    m_cursor_move_pos = QPoint();
            }
        }

        m_mouse_button_pressed = false;

        switch (margin)
        {
        case HTCAPTION:
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
        case HTCAPTION:
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
        case HTCAPTION:
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

bool QGoodWindow::nativeEvent(const QByteArray &eventType, void *message, qgoodintptr *result)
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
#ifdef QT_VERSION_QT5
            else
            {
                //Fix no mouse event after moving mouse from resize borders.
                QEvent event(QEvent::MouseMove);
                QApplication::sendEvent(this, &event);
            }
#endif
        }
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}

#ifdef Q_OS_WIN
void QGoodWindow::initGW()
{
    setProperty("_q_embedded_native_parent_handle", WId(m_hwnd));
    QMainWindow::setWindowFlags(Qt::FramelessWindowHint);

    QEvent event(QEvent::EmbeddingControl);
    QApplication::sendEvent(this, &event);
}

void QGoodWindow::destroyGW()
{
    QMainWindow::close();
#ifdef QT_VERSION_QT5
    QMainWindow::destroy(true, true);
#endif
#ifdef QT_VERSION_QT6
    QMainWindow::destroy(false, false);
#endif
}

void QGoodWindow::closeGW()
{
    if (m_shadow)
        delete m_shadow;
    if (m_helper_widget)
        delete m_helper_widget;
#ifdef QT_VERSION_QT6
    if (m_helper_window)
        delete m_helper_window;
#endif

    qDeleteAll(findChildren<QGoodDialog*>());

    destroyGW();

    SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
}

void QGoodWindow::setWindowStateWin()
{
    if (m_window_state.testFlag(Qt::WindowFullScreen))
        showFullScreen();
    else if (m_window_state.testFlag(Qt::WindowMaximized))
        showMaximized();
    else if (m_window_state.testFlag(Qt::WindowMinimized))
        showMinimized();
    else if (m_window_state.testFlag(Qt::WindowNoState))
        showNormal();
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

        int border_width = 0;

        if (gw->m_is_using_system_borders && gw->windowState().testFlag(Qt::WindowNoState))
            border_width = BORDERWIDTH();

        QSize minimum = gw->minimumSize();

        QSize sizeHint = gw->minimumSizeHint();

        mmi->ptMinTrackSize.x = qFloor(qMax(minimum.width(), sizeHint.width()) *
                                       gw->m_pixel_ratio) + border_width * 2;
        mmi->ptMinTrackSize.y = qFloor(qMax(minimum.height(), sizeHint.height()) *
                                       gw->m_pixel_ratio) + border_width;

        QSize maximum = gw->maximumSize();

        mmi->ptMaxTrackSize.x = qFloor(maximum.width() * gw->m_pixel_ratio) + border_width * 2;
        mmi->ptMaxTrackSize.y = qFloor(maximum.height() * gw->m_pixel_ratio) + border_width;

        break;
    }
    case WM_CLOSE:
    {
        //Send Qt QCloseEvent to the window,
        //which allows to accept or reject the window close.
        gw->m_self_generated_close_event = true;
        QCloseEvent event;
        QApplication::sendEvent(gw, &event);
        gw->m_self_generated_close_event = false;

        if (!event.isAccepted())
            return 0;

        gw->hide();

        gw->m_closed = true;

        //Begin the cleanup.

        gw->closeGW();

        if (gw->testAttribute(Qt::WA_DeleteOnClose))
            delete gw;

        //End the cleanup.

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

        if (FIXED_SIZE(gw))
        {
            //If have fixed size, then only HTCAPTION hit test is valid,
            //which means that only the title bar click and move is valid.

            if (lresult != HTNOWHERE
                    && lresult != HTSYSMENU
                    && lresult != HTMINBUTTON
                    && lresult != HTMAXBUTTON
                    && lresult != HTCLOSE
                    && lresult != HTCAPTION)
                lresult = HTNOWHERE;

            return lresult;
        }
        else if (FIXED_WIDTH(gw))
        {
            if (lresult != HTNOWHERE
                    && lresult != HTSYSMENU
                    && lresult != HTMINBUTTON
                    && lresult != HTMAXBUTTON
                    && lresult != HTCLOSE
                    && lresult != HTCAPTION
                    && lresult != HTTOP
                    && lresult != HTBOTTOM)
                lresult = HTNOWHERE;

            return lresult;
        }
        else if (FIXED_HEIGHT(gw))
        {
            if (lresult != HTNOWHERE
                    && lresult != HTSYSMENU
                    && lresult != HTMINBUTTON
                    && lresult != HTMAXBUTTON
                    && lresult != HTCLOSE
                    && lresult != HTCAPTION
                    && lresult != HTLEFT
                    && lresult != HTRIGHT)
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

        qintptr button = gw->ncHitTest(pos.x(), pos.y());

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
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        const int border_width = (gw->m_is_win_11_or_greater &&
                                  gw->windowState().testFlag(Qt::WindowNoState)) ? BORDERWIDTH() : 0;

        pos.setX(qFloor(gw->x() * gw->m_pixel_ratio) + border_width + pos.x());
        pos.setY(qFloor(gw->y() * gw->m_pixel_ratio) + pos.y());

        qintptr button = gw->ncHitTest(pos.x(), pos.y());

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
    case WM_NCLBUTTONDBLCLK:
    {
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        qintptr button = gw->ncHitTest(pos.x(), pos.y());

        if (button == HTSYSMENU)
        {
            QTimer::singleShot(0, gw, &QGoodWindow::close);
            return 0;
        }

        break;
    }
    case WM_NCLBUTTONDOWN:
    {
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        qintptr button = gw->ncHitTest(pos.x(), pos.y());

        if (button == HTSYSMENU)
        {
            gw->iconClicked();
            return 0;
        }

        bool valid_caption_button = gw->buttonPress(button);

        if (valid_caption_button)
            return 0;

        break;
    }
    case WM_NCLBUTTONUP:
    {
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        qintptr button = gw->ncHitTest(pos.x(), pos.y());

        if (button == HTSYSMENU)
        {
            return 0;
        }

        bool valid_caption_button = gw->buttonRelease(gw->m_caption_button_pressed,
                                                      (button == gw->m_caption_button_pressed));

        if (valid_caption_button)
            return 0;

        break;
    }
    case WM_LBUTTONDOWN:
    {
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        const int border_width = (gw->m_is_win_11_or_greater &&
                                  gw->windowState().testFlag(Qt::WindowNoState)) ? BORDERWIDTH() : 0;

        pos.setX(qFloor(gw->x() * gw->m_pixel_ratio) + border_width + pos.x());
        pos.setY(qFloor(gw->y() * gw->m_pixel_ratio) + pos.y());

        qintptr button = gw->ncHitTest(pos.x(), pos.y());

        gw->buttonPress(button);

        break;
    }
    case WM_LBUTTONUP:
    {
        QPoint pos = QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        const int border_width = (gw->m_is_win_11_or_greater &&
                                  gw->windowState().testFlag(Qt::WindowNoState)) ? BORDERWIDTH() : 0;

        pos.setX(qFloor(gw->x() * gw->m_pixel_ratio) + border_width + pos.x());
        pos.setY(qFloor(gw->y() * gw->m_pixel_ratio) + pos.y());

        qintptr button = gw->ncHitTest(pos.x(), pos.y());

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

        LRESULT lRet = LRESULT(gw->ncHitTest(x, y));

        if (lRet == HTCAPTION || lRet == HTSYSMENU)
            gw->showContextMenu(x, y);

        break;
    }
    case WM_NCCALCSIZE:
    {
        if (gw->isFullScreen())
            break;

        if (gw->m_is_using_system_borders && !gw->isMaximized())
        {
            const int border_width = BORDERWIDTH();

            RECT *rect = reinterpret_cast<RECT*>(lParam);
            rect->left += border_width;
            rect->bottom -= border_width;
            rect->right -= border_width;
        }

        if (gw->isMaximized())
        {
            //Compensate window client area when maximized,
            //by removing BORDERWIDTH value for all edges.

            const int border_width = BORDERWIDTH();

            InflateRect(reinterpret_cast<RECT*>(lParam), -border_width, -border_width);
        }

        //Make the whole window as client area.
        return 0;
    }
#ifdef QT_VERSION_QT5
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

        if (state != gw->m_last_state)
        {
            //If window state changed.

            gw->m_window_state = state;

            if (gw->m_last_state == Qt::WindowMinimized)
            {
                QTimer::singleShot(0, gw, [=]{
                    gw->m_self_generated_show_event = true;
                    QShowEvent event;
                    QApplication::sendEvent(gw, &event);
                    gw->m_self_generated_show_event = false;
                });
            }

            QWindowStateChangeEvent event(gw->m_last_state);
            QApplication::sendEvent(gw, &event);

            gw->m_last_state = state;

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
            if (!gw->m_main_window->isVisible())
            {
                QTimer::singleShot(0, gw->m_main_window, [=]{
                    QTimer::singleShot(0, gw, &QGoodWindow::sizeMoveMainWindow);

                    gw->m_main_window->show();

                    gw->updateScreen(gw->screenForWindow(hwnd));

                    //Fix native window frozen state
                    gw->m_main_window->resize(gw->size() * 2);
                    gw->m_main_window->resize(gw->size());

                    gw->m_window_ready_for_resize = true;

                    gw->m_self_generated_show_event = true;
                    QShowEvent event;
                    QApplication::sendEvent(gw, &event);
                    gw->m_self_generated_show_event = false;
                });
            }

#ifdef QT_VERSION_QT6
            if (gw->m_helper_window)
                gw->m_helper_window->show();
#endif
        }
        else if (pwp->flags & SWP_HIDEWINDOW)
        {
            if (gw->focusWidget())
            {
                //If app have a valid focused widget,
                //save them to restore focus later.

                gw->m_focus_widget = gw->focusWidget();
            }

            if (gw->m_shadow)
                gw->m_shadow->hide();

#ifdef QT_VERSION_QT6
            if (gw->m_helper_window)
                gw->m_helper_window->hide();
#endif

            QHideEvent event;
            QApplication::sendEvent(gw, &event);
        }

        if (pwp->flags == (SWP_NOSIZE + SWP_NOMOVE))
        {
            //Activate window to fix no activation
            //problem when QGoodWindow isn't shown initially in
            //active state.

            if (gw->isMinimized() || !gw->isVisible())
                break;

            gw->m_main_window->activateWindow();

            if (gw->windowState().testFlag(Qt::WindowNoState))
            {
                if (gw->m_shadow)
                    gw->m_shadow->showLater();
            }
        }

        break;
    }
#ifdef QT_VERSION_QT5
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
        Q_EMIT gw->windowTitleChanged(gw->windowTitle());
        break;
    }
    case WM_SETICON:
    {
        Q_EMIT gw->windowIconChanged(gw->windowIcon());
        break;
    }
    case WM_ENTERSIZEMOVE:
    {
        gw->m_timer_move->stop();

        break;
    }
    case WM_EXITSIZEMOVE:
    {
        if (gw->windowHandle()->screen() != gw->screenForWindow(hwnd))
            gw->m_timer_move->start();

        break;
    }
    case WM_DPICHANGED:
    {
        gw->m_timer_move->stop();

        //Wait to refresh screen pixel ratio information.
        qApp->processEvents();

        QScreen *screen = gw->screenForWindow(hwnd);

        if (!screen)
            break;

        gw->screenChangeMoveWindow(screen);
        gw->updateScreen(screen);
        gw->windowScaleChanged();

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
        if (QGoodWindowUtils::isWinXOrGreater(10, 0, 0)) //Windows 10 or later.
        {
            if (QString::fromWCharArray(LPCWSTR(lParam)) == "ImmersiveColorSet")
            {
                QTimer::singleShot(0, gw, &QGoodWindow::themeChanged);
            }
        }

        break;
    }
#ifdef QT_VERSION_QT5
    case WM_THEMECHANGED:
    case WM_DWMCOMPOSITIONCHANGED:
    {
        //Send the window change event to m_helper_widget,
        //this hack corrects the background color when switching between
        //Windows composition modes or system themes.
        SendMessageW(HWND(gw->m_helper_widget->winId()), message, 0, 0);

        if (QtWin::isCompositionEnabled())
        {
            QTimer::singleShot(100, gw, [=]{
                gw->enableCaption();
                gw->frameChanged();
                SetWindowRgn(hwnd, nullptr, TRUE);
            });
        }
        else
        {
            QTimer::singleShot(100, gw, [=]{
                gw->disableCaption();
                gw->frameChanged();
            });
        }

        QTimer::singleShot(100, gw, [=]{
            gw->sizeMoveWindow();
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

    qgoodintptr result = 0;

    bool return_value = gw->nativeEvent(QByteArray(), &msg, &result);

    if (return_value)
        return result;

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void QGoodWindow::handleActivation()
{
    if (m_shadow)
    {
        if (windowState().testFlag(Qt::WindowNoState))
        {
            m_shadow->setActive(true);

            //If in "normal" state, make shadow visible.
            if (isVisible())
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
        //set the focus to this widget.
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
    if (!isVisible() || isMinimized())
        return;

    if (windowState().testFlag(Qt::WindowNoState))
        m_rect_normal = geometry();

#ifdef QT_VERSION_QT5
    setWindowMask();
#endif

    sizeMoveMainWindow();

    moveShadow();
}

void QGoodWindow::sizeMoveMainWindow()
{
    bool opengl_adjust = false;

#if defined QT_VERSION_QT5 && QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QWidgetList list;
    if (m_central_widget)
    {
        list.append(m_central_widget);
        list.append(m_central_widget->findChildren<QWidget*>());
    }

    for (QWidget *widget : list)
    {
        QMetaObject *obj = const_cast<QMetaObject*>(widget->metaObject());

        while (obj)
        {
            if (obj->className() == QStringLiteral("QOpenGLWidget"))
            {
                opengl_adjust = true;
                break;
            }
            else if (obj->className() == QStringLiteral("QQuickWidget"))
            {
                opengl_adjust = true;
                break;
            }

            obj = const_cast<QMetaObject*>(obj->superClass());
        }

        if (opengl_adjust)
            break;
    }
#endif
    //Move proxy widget.
    int y = (opengl_adjust && (!qFuzzyCompare(m_pixel_ratio, qreal(1))) ? height() : 0);
    m_proxy_widget->setGeometry(0, y, width(), height());

    HWND hwnd = HWND(m_main_window->winId());

    RECT win_rect;
    GetClientRect(m_hwnd, &win_rect);
    int w = win_rect.right - win_rect.left;
    int h = win_rect.bottom - win_rect.top;

    //Move main window widget inside native main window.
    SetWindowPos(hwnd, nullptr, 0, 0, w, h, SWP_NOACTIVATE);
}

void QGoodWindow::setWindowMask()
{
#ifdef QT_VERSION_QT5
    if (!QtWin::isCompositionEnabled())
    {
        QRect window_rect = rect();

        window_rect.setWidth(qFloor(window_rect.width() * m_pixel_ratio));
        window_rect.setHeight(qFloor(window_rect.height() * m_pixel_ratio));

        const int border_width = BORDERWIDTH();

        if (isMaximized())
            window_rect.moveTopLeft(QPoint(border_width, border_width));

        SetWindowRgn(m_hwnd, QtWin::toHRGN(window_rect), TRUE);
    }
    else
    {
        SetWindowRgn(m_hwnd, nullptr, TRUE);
    }
#endif
#ifdef QT_VERSION_QT6
    SetWindowRgn(m_hwnd, nullptr, TRUE);
#endif
}

void QGoodWindow::moveShadow()
{
    if (!m_shadow)
        return;

    if (!windowState().testFlag(Qt::WindowNoState))
        return;

    const int shadow_width = m_shadow->shadowWidth();
    const QPoint adjusted_pos = screenAdjustedPos();

    int pos_x = x() - shadow_width + adjusted_pos.x();
    int pos_y = y() - shadow_width + adjusted_pos.y();
    int w = width() + shadow_width * 2;
    int h = height() + shadow_width * 2;

    m_shadow->setGeometry(pos_x, pos_y, w, h);

    m_shadow->setActive(isActiveWindow());
}

void QGoodWindow::screenChangeMoveWindow(QScreen *screen)
{
    if (!screen)
        return;

    QRect screen_geom = screen->geometry();
    qreal screen_pixel_ratio = screen->devicePixelRatio();
    screen_geom.setWidth(qFloor(screen_geom.width() * screen_pixel_ratio));
    screen_geom.setHeight(qFloor(screen_geom.height() * screen_pixel_ratio));

    RECT rect;
    GetWindowRect(m_hwnd, &rect);

    QRect window_geom;
    window_geom.moveTop(rect.top);
    window_geom.moveLeft(rect.left);
    window_geom.setWidth(rect.right - rect.left);
    window_geom.setHeight(rect.bottom - rect.top);

    QPoint center_pos = window_geom.center();
    int border_width = m_is_using_system_borders ? BORDERWIDTH() : 0;

    QSize window_size = size();
    window_size.setWidth(qFloor(window_size.width() * screen_pixel_ratio) + border_width * 2);
    window_size.setHeight(qFloor(window_size.height() * screen_pixel_ratio) + border_width);
    window_geom.setSize(window_size);

    //Keep X position.
    center_pos.setY(window_geom.center().y());

    const int top_dist = qAbs(screen_geom.top() - center_pos.y());
    const int bottom_dist = qAbs(screen_geom.bottom() - center_pos.y());
    const int left_dist = qAbs(screen_geom.left() - center_pos.x());
    const int right_dist = qAbs(screen_geom.right() - center_pos.x());

    const int margin = 10;

    QRect center_rect;
    center_rect.moveCenter(center_pos);
    center_rect.adjust(-margin, -margin, margin, margin);

    if (!screen_geom.contains(center_rect))
    {
        if (left_dist < right_dist)
        {
            if (left_dist < top_dist && left_dist < bottom_dist)
            {
                center_pos.setX(screen_geom.left() + margin);
            }
            else if (top_dist < bottom_dist)
            {
                center_pos.setY(screen_geom.top() + margin);
            }
            else
            {
                center_pos.setY(screen_geom.bottom() - margin);
            }
        }
        else if (left_dist > right_dist)
        {
            if (right_dist < top_dist && right_dist < bottom_dist)
            {
                center_pos.setX(screen_geom.right() - margin);
            }
            else if (top_dist < bottom_dist)
            {
                center_pos.setY(screen_geom.top() + margin);
            }
            else
            {
                center_pos.setY(screen_geom.bottom() - margin);
            }
        }
    }

    window_geom.moveCenter(center_pos);

    m_pixel_ratio = screen_pixel_ratio;

    SetWindowPos(m_hwnd, nullptr, window_geom.x(), window_geom.y(),
                 window_geom.width(), window_geom.height(), SWP_NOACTIVATE);

    moveShadow();
}

void QGoodWindow::updateScreen(QScreen *screen)
{
    if (!screen)
        return;

    m_pixel_ratio = screen->devicePixelRatio();
    setCurrentScreen(screen);
}

void QGoodWindow::setCurrentScreen(QScreen *screen)
{
    windowHandle()->setScreen(screen);

    m_main_window->windowHandle()->setScreen(screen);

    if (m_shadow)
        m_shadow->windowHandle()->setScreen(screen);
}

void QGoodWindow::windowScaleChanged()
{
#ifdef QT_VERSION_QT6
    internalWidgetResize();
#endif

    //Fix widget scale bug.
    resize(width() + 1, height() + 1);
    qApp->processEvents();
    resize(width() - 1, height() - 1);
    qApp->processEvents();

    sizeMoveWindow();
    moveShadow();
}

void QGoodWindow::internalWidgetResize()
{
    if (!windowState().testFlag(Qt::WindowNoState))
        return;

    if (!m_window_ready_for_resize)
        return;

    if (m_self_generated_resize_event)
        return;

    const QSize current_size = size();
    QSize mw_size = m_main_window->size();

#ifdef QT_VERSION_QT5
    mw_size.setWidth(qFloor(mw_size.width() / m_pixel_ratio));
    mw_size.setHeight(qFloor(mw_size.height() / m_pixel_ratio));
#endif

    if (current_size.width() < qFloor(mw_size.width() * m_pixel_ratio) ||
        current_size.width() > qCeil(mw_size.width() * m_pixel_ratio))
        return;

    if (current_size.height() < qFloor(mw_size.height() * m_pixel_ratio) ||
        current_size.height() > qCeil(mw_size.height() * m_pixel_ratio))
        return;

    resize(mw_size);
}

QScreen *QGoodWindow::screenForWindow(HWND hwnd)
{
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitor_info;
    memset(&monitor_info, 0, sizeof(MONITORINFO));
    monitor_info.cbSize = sizeof(MONITORINFO);

    GetMonitorInfoW(monitor, &monitor_info);

    QPoint top_left;
    top_left.setX(monitor_info.rcMonitor.left);
    top_left.setY(monitor_info.rcMonitor.top);

    for (QScreen *screen : qApp->screens())
    {
        if (screen->geometry().topLeft() == top_left)
        {
            return screen;
        }
    }

    return nullptr;
}

QPoint QGoodWindow::screenAdjustedPos()
{
    const QRect screen_geom = windowHandle()->screen()->geometry();

    int adjust_x = qFloor(screen_geom.x() - (screen_geom.x() / m_pixel_ratio));
    int adjust_y = qFloor(screen_geom.y() - (screen_geom.y() / m_pixel_ratio));

    return QPoint(adjust_x, adjust_y);
}

QScreen *QGoodWindow::screenForPoint(const QPoint &pos)
{
    POINT point;
    point.x = pos.x();
    point.y = pos.y();

    HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitor_info;
    memset(&monitor_info, 0, sizeof(MONITORINFO));
    monitor_info.cbSize = sizeof(MONITORINFO);

    GetMonitorInfoW(monitor, &monitor_info);

    QPoint top_left;
    top_left.setX(monitor_info.rcMonitor.left);
    top_left.setY(monitor_info.rcMonitor.top);

    for (QScreen *screen : qApp->screens())
    {
        if (screen->geometry().topLeft() == top_left)
        {
            return screen;
        }
    }

    return nullptr;
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

    if (FIXED_SIZE(this))
    {
        SetMenuItemInfoW(menu, SC_SIZE, FALSE, &mi);
    }

    int cmd = int(TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
                                 x, y, 0, m_hwnd, nullptr));

    if (cmd)
    {
        SendMessageW(m_hwnd, WM_SYSCOMMAND, WPARAM(cmd), 0);
    }
}

void QGoodWindow::showContextMenu()
{
    int pos_x = x();
    int pos_y = y() + titleBarHeight();

    if (m_is_using_system_borders && windowState().testFlag(Qt::WindowNoState))
        pos_x += BORDERWIDTH();

    pos_x = qFloor(pos_x * m_pixel_ratio);
    pos_y = qFloor(pos_y * m_pixel_ratio);

    showContextMenu(pos_x, pos_y);
}

QWidget *QGoodWindow::bestParentForModalWindow()
{
    return new QGoodWindowUtils::ParentWindow(this);
}

void QGoodWindow::moveCenterWindow(QWidget *widget)
{
    QGoodWindow *child_gw = qobject_cast<QGoodWindow*>(widget);

    const int title_bar_height = titleBarHeight();

    int border_width = 0;

    if (m_is_using_system_borders && windowState().testFlag(Qt::WindowNoState))
        border_width = BORDERWIDTH();

    QRect rect;

    if (!isMinimized() && isVisible())
        rect = frameGeometry();
    else
        rect = m_window_handle->screen()->availableGeometry();

    QRect screen_rect = m_window_handle->screen()->availableGeometry();

    QRect window_rect;

    if (child_gw)
        window_rect = child_gw->frameGeometry();
    else
        window_rect = widget->frameGeometry();

    window_rect.moveCenter(rect.center());

    if (child_gw)
    {
        window_rect.moveLeft(qMax(window_rect.left(), screen_rect.left()));
        window_rect.moveTop(qMax(window_rect.top(), screen_rect.top()));
        window_rect.moveRight(qMin(window_rect.right(), screen_rect.right()));
        window_rect.moveBottom(qMin(window_rect.bottom(), screen_rect.bottom()));
    }
    else
    {
        window_rect.moveLeft(qMax(window_rect.left(), screen_rect.left() + border_width));
        window_rect.moveTop(qMax(window_rect.top() + titleBarHeight(), screen_rect.top() + title_bar_height));
        window_rect.moveRight(qMin(window_rect.right(), screen_rect.right() - border_width));
        window_rect.moveBottom(qMin(window_rect.bottom(), screen_rect.bottom() - border_width));
    }

    if (child_gw)
        child_gw->setGeometry(window_rect);
    else
        widget->setGeometry(window_rect);
}

bool QGoodWindow::winButtonHover(qintptr button)
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

void QGoodWindow::iconClicked()
{
    if (!m_is_menu_visible)
    {
        m_is_menu_visible = true;

        m_timer_menu->start();

        showContextMenu();

        QTimer::singleShot(qApp->doubleClickInterval(), this, [=]{
            m_is_menu_visible = false;
        });
    }

    if (m_timer_menu->isActive())
        QTimer::singleShot(0, this, &QGoodWindow::close);
}
#endif
#ifdef Q_OS_LINUX
void QGoodWindow::setCursorForCurrentPos()
{
    const QPoint cursor_pos = QCursor::pos();
    const int margin = int(ncHitTest(qFloor(cursor_pos.x() * m_pixel_ratio), qFloor(cursor_pos.y() * m_pixel_ratio)));

    m_cursor_pos = cursor_pos;
    m_margin = margin;

    QWidget *widget = QApplication::widgetAt(cursor_pos);

    if (!widget)
        return;

    Display *dpy = QX11Info::display();

    switch (margin)
    {
    case HTTOPLEFT:
    {
        Cursor cursor;

        if (!FIXED_SIZE(this))
            cursor = XCreateFontCursor(dpy, XC_top_left_corner);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case HTTOP:
    {
        Cursor cursor;

        if (!FIXED_HEIGHT(this))
            cursor = XCreateFontCursor(dpy, XC_top_side);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case HTTOPRIGHT:
    {
        Cursor cursor;

        if (!FIXED_SIZE(this))
            cursor = XCreateFontCursor(dpy, XC_top_right_corner);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case HTLEFT:
    {
        Cursor cursor;

        if (!FIXED_WIDTH(this))
            cursor = XCreateFontCursor(dpy, XC_left_side);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case HTRIGHT:
    {
        Cursor cursor;

        if (!FIXED_WIDTH(this))
            cursor = XCreateFontCursor(dpy, XC_right_side);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case HTBOTTOMLEFT:
    {
        Cursor cursor;

        if (!FIXED_SIZE(this))
            cursor = XCreateFontCursor(dpy, XC_bottom_left_corner);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case HTBOTTOM:
    {
        Cursor cursor;

        if (!FIXED_HEIGHT(this))
            cursor = XCreateFontCursor(dpy, XC_bottom_side);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case HTBOTTOMRIGHT:
    {
        Cursor cursor;

        if (!FIXED_SIZE(this))
            cursor = XCreateFontCursor(dpy, XC_bottom_right_corner);
        else
            cursor = XCreateFontCursor(dpy, XC_arrow);

        XDefineCursor(dpy, Window(widget->winId()), cursor);

        XFlush(dpy);

        XFreeCursor(dpy, cursor);

        break;
    }
    case HTCAPTION:
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
    case HTNOWHERE:
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

    if (margin == HTNOWHERE)
        return;

    if (FIXED_SIZE(this) && margin != HTCAPTION)
        return;

    QPoint cursor_pos = QPoint(qFloor(m_cursor_pos.x() * m_pixel_ratio), qFloor(m_cursor_pos.y() * m_pixel_ratio));

    XClientMessageEvent xmsg;
    memset(&xmsg, 0, sizeof(XClientMessageEvent));

    xmsg.type = ClientMessage;
    xmsg.window = Window(winId());
    xmsg.message_type = XInternAtom(QX11Info::display(), "_NET_WM_MOVERESIZE", False);
    xmsg.format = 32;
    xmsg.data.l[0] = long(cursor_pos.x());
    xmsg.data.l[1] = long(cursor_pos.y());

    if (margin == HTCAPTION)
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

    QTimer::singleShot(qApp->doubleClickInterval(), this, [=]{
        m_resize_move_started = true;
    });
}

void QGoodWindow::sizeMove()
{
    if (!m_resize_move)
        return;

    m_resize_move = false;

    const int margin = m_margin;

    if (margin == HTCAPTION)
    {
        QTimer::singleShot(0, this, [=]{
            startSystemMoveResize();
        });
    }
    else
    {
        startSystemMoveResize();
    }
}

void QGoodWindow::sizeMoveBorders()
{
    if (!windowState().testFlag(Qt::WindowNoState))
        return;

    const int border_width = BORDERWIDTHDPI;

    QRegion visible_rgn;

    for (const QScreen *screen : qApp->screens())
    {
        visible_rgn += screen->availableGeometry();
    }

    QRect visible_geom = visible_rgn.boundingRect();
    QRect frame_geom = frameGeometry();
    frame_geom.adjust(-border_width, -border_width, border_width, border_width);

    {
        int x = frame_geom.left();
        int y = frame_geom.top();
        int w = frame_geom.width();
        int h = border_width;

        QRect top_rect = QRect(x, y, w, h).intersected(visible_geom);

        m_top_shadow->move(top_rect.topLeft());
        m_top_shadow->setFixedSize(top_rect.size());
    }

    {
        int x = frame_geom.left();
        int y = frame_geom.bottom() - border_width;
        int w = frame_geom.width();
        int h = border_width;

        QRect bottom_rect = QRect(x, y, w, h).intersected(visible_geom);

        m_bottom_shadow->move(bottom_rect.topLeft());
        m_bottom_shadow->setFixedSize(bottom_rect.size());
    }

    {
        int x = frame_geom.left();
        int y = frame_geom.top() + border_width;
        int w = border_width;
        int h = frame_geom.height() - border_width * 2;

        QRect left_rect = QRect(x, y, w, h).intersected(visible_geom);

        m_left_shadow->move(left_rect.topLeft());
        m_left_shadow->setFixedSize(left_rect.size());
    }

    {
        int x = frame_geom.left() + frame_geom.width() - border_width;
        int y = frame_geom.top() + border_width;
        int w = border_width;
        int h = frame_geom.height() - border_width * 2;

        QRect right_rect = QRect(x, y, w, h).intersected(visible_geom);

        m_right_shadow->move(right_rect.topLeft());
        m_right_shadow->setFixedSize(right_rect.size());
    }
}
#endif
#ifdef Q_OS_MAC
void QGoodWindow::notificationReceiver(const QByteArray &notification)
{
    if (notification == "NSWindowWillEnterFullScreenNotification")
    {
        m_on_animate_event = true;
        macOSNative::setStyle(long(winId()), macOSNative::StyleType::Fullscreen);
    }
    else if (notification == "NSWindowDidExitFullScreenNotification")
    {
        macOSNative::setStyle(long(winId()), macOSNative::StyleType::NoState);
        m_on_animate_event = false;
    }
    else if (notification == "AppleInterfaceThemeChangedNotification")
    {
        QTimer::singleShot(0, this, &QGoodWindow::themeChanged);
    }
}
#endif

#ifdef QGOODWINDOW
qintptr QGoodWindow::ncHitTest(int pos_x, int pos_y)
{
    if (isFullScreen())
    {
        //If on full screen, the whole window can be clicked.
        return HTNOWHERE;
    }

    int border_width = 0;

#ifdef Q_OS_WIN
    if (windowState().testFlag(Qt::WindowNoState))
    {
        if (m_is_using_system_borders)
            border_width = qFloor(BORDERWIDTH() / m_pixel_ratio); //in pixels.
        else
            border_width = qFloor(1 * m_pixel_ratio); //in pixels.
    }
#endif

    //Get the point coordinates for the hit test.
    const QPoint cursor_pos = QPoint(qFloor(pos_x / m_pixel_ratio), qFloor(pos_y / m_pixel_ratio));

    //Get the mapped point coordinates for the hit test without border width.
    const QPoint cursor_pos_map = QPoint(cursor_pos.x() - x() - border_width, cursor_pos.y() - y());

#ifdef Q_OS_WIN
    for (QSizeGrip *size_grip : findChildren<QSizeGrip*>())
    {
        if (size_grip->isEnabled() &&
                !size_grip->window()->windowFlags().testFlag(Qt::SubWindow))
        {
            QPoint cursor_pos_map_widget = size_grip->parentWidget()->mapFromGlobal(cursor_pos);

            QPoint adjusted_pos = screenAdjustedPos();

            cursor_pos_map_widget.setX(cursor_pos_map_widget.x() + adjusted_pos.x());
            cursor_pos_map_widget.setY(cursor_pos_map_widget.y() + adjusted_pos.y());

            if (size_grip->geometry().contains(cursor_pos_map_widget))
                return HTBOTTOMRIGHT;
        }
    }
#endif

    const int title_bar_height = titleBarHeight();

#ifdef Q_OS_WIN
    const int icon_width = iconWidth();
#endif

    //Get the window rectangle.
#ifndef Q_OS_MAC
    QRect window_rect = frameGeometry();
#else
    //Get the correct window rectangle for macOS.
    QRect window_rect;

    if (isVisible())
    {
        int x, y, w, h;
        macOSNative::frameGeometry(long(winId()), &x, &y, &w, &h);
        window_rect = QRect(x, y, w, h);
    }
#endif

    //Determine if the hit test is for resizing. Default middle (1,1).
    int row = 1;
    int col = 1;
    bool on_resize_border = false;

    //Determine if the point is at the top or bottom of the window.
    if (cursor_pos.y() < window_rect.top() + title_bar_height)
    {
        on_resize_border = (cursor_pos.y() < (window_rect.top() + border_width));
        row = 0; //top border.
    }
    else if (cursor_pos.y() > window_rect.bottom() - border_width)
    {
        row = 2; //bottom border.
    }

    //Determine if the point is at the left or right of the window.
    if (cursor_pos.x() < window_rect.left() + border_width)
    {
        col = 0; //left border.
    }
    else if (cursor_pos.x() > window_rect.right() - border_width)
    {
        col = 2; //right border.
    }
    else if (row == 0 && !on_resize_border)
    {
        if (m_cls_mask.contains(cursor_pos_map))
            return HTCLOSE; //title bar close button.
        else if (m_max_mask.contains(cursor_pos_map))
            return HTMAXBUTTON; //title bar maximize button.
        else if (m_min_mask.contains(cursor_pos_map))
            return HTMINBUTTON; //title bar minimize button.
#ifdef Q_OS_WIN
        else if (cursor_pos_map.x() <= icon_width)
            return HTSYSMENU; //title bar icon.
#endif
        else if (m_title_bar_mask.contains(cursor_pos_map))
            return HTNOWHERE; //user title bar mask.
    }

    //Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
    qintptr hitTests[3][3] =
    {
        {HTTOPLEFT, on_resize_border ? HTTOP : HTCAPTION, HTTOPRIGHT},
        {HTLEFT, HTNOWHERE, HTRIGHT},
        {HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT},
    };

    return hitTests[row][col];
}

void QGoodWindow::buttonEnter(qintptr button)
{
    if (button == -1)
        return;

    if (button == m_last_caption_button_hovered)
        return;

    m_last_caption_button_hovered = button;

    switch (button)
    {
    case HTMINBUTTON:
    {
        Q_EMIT captionButtonStateChanged(CaptionButtonState::MinimizeHoverEnter);

        break;
    }
    case HTMAXBUTTON:
    {
        Q_EMIT captionButtonStateChanged(CaptionButtonState::MaximizeHoverEnter);

        break;
    }
    case HTCLOSE:
    {
        Q_EMIT captionButtonStateChanged(CaptionButtonState::CloseHoverEnter);

        break;
    }
    default:
        break;
    }
}

void QGoodWindow::buttonLeave(qintptr button)
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
        Q_EMIT captionButtonStateChanged(CaptionButtonState::MinimizeHoverLeave);

        break;
    }
    case HTMAXBUTTON:
    {
        Q_EMIT captionButtonStateChanged(CaptionButtonState::MaximizeHoverLeave);

        break;
    }
    case HTCLOSE:
    {
        Q_EMIT captionButtonStateChanged(CaptionButtonState::CloseHoverLeave);

        break;
    }
    default:
        break;
    }
}

bool QGoodWindow::buttonPress(qintptr button)
{
    if (button == -1)
        return false;

    switch (button)
    {
    case HTMINBUTTON:
    {
        m_is_caption_button_pressed = true;
        m_caption_button_pressed = HTMINBUTTON;

        Q_EMIT captionButtonStateChanged(CaptionButtonState::MinimizePress);

        activateWindow();

        return true;
    }
    case HTMAXBUTTON:
    {
        m_is_caption_button_pressed = true;
        m_caption_button_pressed = HTMAXBUTTON;

        Q_EMIT captionButtonStateChanged(CaptionButtonState::MaximizePress);

        activateWindow();

        return true;
    }
    case HTCLOSE:
    {
        m_is_caption_button_pressed = true;
        m_caption_button_pressed = HTCLOSE;

        Q_EMIT captionButtonStateChanged(CaptionButtonState::ClosePress);

        activateWindow();

        return true;
    }
    default:
        break;
    }

    return false;
}

bool QGoodWindow::buttonRelease(qintptr button, bool valid_click)
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
        Q_EMIT captionButtonStateChanged(CaptionButtonState::MinimizeRelease);

        if (valid_click)
        {
            buttonLeave(button);
            Q_EMIT captionButtonStateChanged(CaptionButtonState::MinimizeClicked);
            m_hover_timer->start();
        }

        return true;
    }
    case HTMAXBUTTON:
    {
        Q_EMIT captionButtonStateChanged(CaptionButtonState::MaximizeRelease);

        if (valid_click)
        {
            buttonLeave(button);
            Q_EMIT captionButtonStateChanged(CaptionButtonState::MaximizeClicked);
            m_hover_timer->start();
        }

        return true;
    }
    case HTCLOSE:
    {
        Q_EMIT captionButtonStateChanged(CaptionButtonState::CloseRelease);

        if (valid_click)
        {
            buttonLeave(button);
            Q_EMIT captionButtonStateChanged(CaptionButtonState::CloseClicked);
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
