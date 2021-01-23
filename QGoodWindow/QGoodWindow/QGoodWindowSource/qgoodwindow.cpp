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

#include "QGoodWindowSource/qgoodwindow.h"
#include "QGoodWindowSource/shadow.h"

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

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include "QGoodWindowSource/common.h"
#include "QGoodWindowSource/winwidget.h"

#define BORDERWIDTH (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER))

static QColor m_clear_color = QColor(Qt::white);

#endif

#ifdef Q_OS_LINUX

#include <QtX11Extras/QX11Info>
#include <QtTest/QtTest>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

#define SHADOW_WINDOW_COUNT 8

#define CURSORSIZE 8

#endif

#ifdef Q_OS_MAC

#include "macosnative.h"

Notification notification;

#define NO_WHERE -1
#define TOP_LEFT 0
#define TOP 1
#define TOP_RIGHT 2
#define LEFT 7
#define RIGHT 3
#define BOTTOM_LEFT 6
#define BOTTOM 5
#define BOTTOM_RIGHT 4
#define TITLE_BAR 8

#endif

QGoodWindow::QGoodWindow(QWidget *parent) : QMainWindow(parent)
{
#if defined Q_OS_WIN || defined Q_OS_LINUX
    m_fixed_size = false;
#endif
#ifdef Q_OS_WIN
    m_is_full_screen = false;

    m_last_shadow_state_hidden = true;
    m_last_is_visible = false;
    m_last_minimized = false;
    m_last_maximized = false;
    m_last_fullscreen = false;
    m_window_ready = false;
    m_last_state_is_inactive = false;
    m_dialog_visible_count = 0;

    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wcx;
    memset(&wcx, 0, sizeof(WNDCLASSEXW));
    wcx.cbSize = sizeof(WNDCLASSEXW);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.hInstance = hInstance;
    wcx.lpfnWndProc = WNDPROC(WndProc);
    wcx.cbClsExtra	= 0;
    wcx.cbWndExtra	= 0;
    wcx.hCursor	= LoadCursor(nullptr, IDC_ARROW);
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

    QScreen *screen = QApplication::primaryScreen();
    m_pixel_ratio = screen->logicalDotsPerInch() / qreal(96);

    m_helper_widget = new QWidget();

    m_win_widget = new WinWidget(m_hwnd, this);

    m_shadow = new Shadow(m_pixel_ratio, m_hwnd);

    m_window_handle = QWindow::fromWinId(WId(m_hwnd));

    QMainWindow::setParent(m_win_widget);
    QMainWindow::setWindowFlags(Qt::Widget);

    connect(qApp, &QApplication::aboutToQuit, this, [=]{
        //Fix QApplication::exit() crash
        SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
    });

    QGridLayout *layout = new QGridLayout(m_win_widget);
    layout->setMargin(0);
    layout->addWidget(this);

    setMargins(qRound(30 * m_pixel_ratio), 0, 0, 0);

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

    for (int i = 0; i < CURSORSIZE; i++)
    {
        const QString name = convertIndexToName(i);
        const QPixmap cursor = openCursor(name);

        m_cursor_list.append(cursor);
    }

    installEventFilter(this);
    setMouseTracking(true);

    setWindowFlags(Qt::FramelessWindowHint);

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
#ifdef Q_OS_MAC
    notification.removeWindow(this);
#endif
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
            gw->m_last_state_is_inactive = false;

            QTimer::singleShot(0, gw, [=]{
                gw->handleActivation();
            });

            break;
        }
        case WA_INACTIVE:
        {
            gw->m_last_state_is_inactive = true;

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
        if ((HWND(wParam) == HWND(gw->m_win_widget->winId())) ||
                gw->m_last_state_is_inactive ||
                gw->m_dialog_visible_count > 0)
        {
            gw->m_last_state_is_inactive = false;

            QTimer::singleShot(0, gw, [=]{
                gw->handleActivation();
            });
        }
        else
        {
            gw->handleActivation();
        }

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

        if (gw->m_shadow)
            gw->m_shadow->hide();

        gw->hide();

        //Do the needed cleanup.

        gw->QMainWindow::setParent(nullptr);

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
            //with means that only the title bar click and move is valid.

            if (lresult != HTNOWHERE && lresult != HTCAPTION)
                lresult = HTNOWHERE;

            return lresult;
        }

        return lresult;
    }
    case WM_CONTEXTMENU:
    {
        //When SHIFT+F10 is pressed.

        int x = gw->x() + (!gw->isMaximized() ? BORDERWIDTH : 0);
        int y = gw->y() + gw->titleBarHeight() - (gw->isMaximized() ? BORDERWIDTH : 0);

        gw->showContextMenu(x, y);

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

            InflateRect(reinterpret_cast<RECT*>(lParam), -BORDERWIDTH, -BORDERWIDTH);
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
    case WM_WINDOWPOSCHANGED:
    {
        if (gw->isVisible() != gw->m_last_is_visible
                || gw->isMinimized() != gw->m_last_minimized
                || gw->isMaximized() != gw->m_last_maximized
                || gw->isFullScreen() != gw->m_last_fullscreen)
        {
            //If window state changed.

            QWindowStateChangeEvent event = QWindowStateChangeEvent(Qt::WindowNoState);
            QApplication::sendEvent(gw, &event);

            gw->m_last_state_is_inactive = (!gw->m_last_is_visible || gw->m_last_minimized);

            gw->m_last_is_visible = gw->isVisible();
            gw->m_last_minimized = gw->isMinimized();
            gw->m_last_maximized = gw->isMaximized();
            gw->m_last_fullscreen = gw->isFullScreen();
        }

        if (!QtWin::isCompositionEnabled() && gw->isMaximized())
        {
            //Hack for prevent window goes to full screen when it's being maximized,
            //enable WS_CAPTION and schedule for disable it,
            //not mantaining WS_CAPTION all the time to prevent undesired painting on window
            //when title or icon of the window is changed when DWM is not enabled.

            gw->enableCaption();
            QTimer::singleShot(0, gw, &QGoodWindow::disableCaption);
        }

        if (gw->isMinimized())
        {
            if (gw->window()->focusWidget())
            {
                //If app going to be minimized,
                //save current focused widget
                //to restore focus later.

                gw->m_focus_widget = gw->window()->focusWidget();
            }
        }

        break;
    }
    case WM_WINDOWPOSCHANGING:
    {
        bool minimized = gw->isMinimized();
        bool maximized = gw->isMaximized();
        bool fullscreen = gw->isFullScreen();

        if (minimized || maximized || fullscreen)
        {
            //Hide shadow if not on "normal" state.

            gw->m_last_shadow_state_hidden = true;

            if (gw->m_window_ready)
                gw->m_shadow->hide();
        }
        else if ((gw->m_last_shadow_state_hidden && !minimized && !maximized && !fullscreen))
        {
            //Show shadow if switching to "normal" state, with delay.

            if (gw->m_window_ready)
                gw->m_shadow->showLater();

            gw->m_last_shadow_state_hidden = false;
        }

        WINDOWPOS *pwp = reinterpret_cast<WINDOWPOS*>(lParam);

        if (pwp->flags & SWP_SHOWWINDOW)
        {
            QShowEvent event = QShowEvent();
            QApplication::sendEvent(gw, &event);

            if (gw->m_fixed_size)
            {
                SetWindowLongPtrW(hwnd, GWL_STYLE, GetWindowLongPtrW(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
            }

            {
                //Restore brush with clear_color.

                HBRUSH brush = HBRUSH(CreateSolidBrush(RGB(m_clear_color.red(),
                                                           m_clear_color.green(),
                                                           m_clear_color.blue())));
                HBRUSH oldbrush = HBRUSH(SetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND, LONG_PTR(brush)));

                DeleteObject(oldbrush);
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            gw->m_window_ready = true;

            if (!minimized && !maximized && !fullscreen)
            {
                //Show shadow if on "normal" state with delay.

                gw->m_shadow->showLater();
            }

            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            QTimer::singleShot(0, gw->m_win_widget, &WinWidget::show);
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
                HBRUSH oldbrush = HBRUSH(SetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND, LONG_PTR(brush)));

                DeleteObject(oldbrush);
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            QHideEvent event = QHideEvent();
            QApplication::sendEvent(gw, &event);

            gw->m_window_ready = false;

            gw->m_shadow->hide();
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
    case WM_SETTINGCHANGE:
    case WM_THEMECHANGED:
    case WM_DWMCOMPOSITIONCHANGED:
    {
        //Send the window change event to m_helper_widget,
        //this hack corrects the background color when switching between
        //Windows composition modes.
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

        for (QWidget *next_focus : m_win_widget->findChildren<QWidget*>())
        {
            if (next_focus->focusPolicy() != Qt::NoFocus)
            {
                //Set focus to first focusable widget.
                have_focusable_widget = true;

                QTimer::singleShot(0, next_focus, [=]{
                    m_focus_widget = next_focus;
                    next_focus->setFocus();
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

    if (!isMaximized() && !isFullScreen())
    {
        //If in "normal" state, make shadow visible.
        QTimer::singleShot(0, m_shadow, [=]{
            m_shadow->setActive(true);
            m_shadow->show();
        });
    }
}

void QGoodWindow::handleDeactivation()
{
    if (!m_win_widget || !m_shadow)
        return;

    m_shadow->setActive(false);

    if (isActiveWindow() &&!isMaximized() && !isFullScreen())
        m_shadow->show();
}

void QGoodWindow::enableCaption()
{
    SetWindowLongPtrW(m_hwnd, GWL_STYLE, GetWindowLongPtrW(m_hwnd, GWL_STYLE) | WS_CAPTION);
}

void QGoodWindow::disableCaption()
{
    SetWindowLongPtrW(m_hwnd, GWL_STYLE, GetWindowLongPtrW(m_hwnd, GWL_STYLE) & ~WS_CAPTION);
}

void QGoodWindow::frameChanged()
{
    SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}
#endif

#ifdef Q_OS_LINUX
QString QGoodWindow::convertIndexToName(int index)
{
    switch (index)
    {
    case 0:
        return QString("/usr/share/icons/DMZ-White/cursors/top_left_corner");
    case 1:
        return QString("/usr/share/icons/DMZ-White/cursors/top_side");
    case 2:
        return QString("/usr/share/icons/DMZ-White/cursors/top_right_corner");
    case 3:
        return QString("/usr/share/icons/DMZ-White/cursors/left_side");
    case 4:
        return QString("/usr/share/icons/DMZ-White/cursors/right_side");
    case 5:
        return QString("/usr/share/icons/DMZ-White/cursors/bottom_left_corner");
    case 6:
        return QString("/usr/share/icons/DMZ-White/cursors/bottom_side");
    case 7:
        return QString("/usr/share/icons/DMZ-White/cursors/bottom_right_corner");
    default:
        break;
    }

    return QString();
}

QPixmap QGoodWindow::openCursor(const QString &name)
{
    QFileInfo file_info = QFileInfo(name);

    QFile file(file_info.absoluteFilePath());

    if (!file.open(QFile::ReadOnly))
        return QPixmap();

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint32 value;
    quint32 header;
    quint32 version;
    quint32 iterations;

    stream >> value >> header >> version >> iterations;

    if (value != 1920295768 && header != 16)
        return QPixmap();

    for (int i = 0; i < int(iterations); i++)
    {
        quint32 type;
        quint32 subtype;
        quint32 position;

        stream >> type >> subtype >> position;

        if (type != 4294770690)
            break;

        file.seek(position);

        quint32 img_header;
        quint32 img_type;
        quint32 img_subtype;
        quint32 img_version;
        quint32 img_width;
        quint32 img_height;
        quint32 img_yhot;
        quint32 img_xhot;
        quint32 img_delay;

        stream >> img_header >> img_type >> img_subtype >> img_version >>
                img_width >> img_height >> img_yhot >> img_xhot >> img_delay;

        if (img_header != 36 || img_type != type || img_subtype != subtype || img_version != 1)
            break;

        QImage cursor = QImage(int(img_width), int(img_height), QImage::Format_ARGB32);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        const int data_size = cursor.sizeInBytes();
#else
        const int data_size = cursor.byteCount();
#endif

        QByteArray img_data = file.read(qint64(data_size));

        memcpy(cursor.bits(), img_data.constData(), uint(img_data.size()));

        QSize img_size = cursor.size();

        if (img_size.width() >= 20 && img_size.width() <= 30 &&
                img_size.height() >= 20 && img_size.height() <= 30)
            return QPixmap::fromImage(cursor);
    }

    return QPixmap();
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

void QGoodWindow::setFixedSize(int w, int h)
{
#if defined Q_OS_WIN || defined Q_OS_LINUX
    m_fixed_size = true;
#endif
#ifdef Q_OS_WIN
    setMinimumSize(w, h);
    setMaximumSize(w, h);
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

#ifdef Q_OS_WIN
    m_win_widget->setMargins(title_bar_height, icon_width, left, right);
#endif

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
        SetWindowLongPtrW(m_hwnd, GWL_STYLE, GetWindowLongPtrW(m_hwnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);
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
#if defined Q_OS_WIN || defined Q_OS_LINUX
    if (m_fixed_size)
        return;
#endif
#ifdef Q_OS_WIN
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
#if defined Q_OS_WIN || defined Q_OS_LINUX
    if (m_fixed_size)
        return;
#endif
#ifdef Q_OS_WIN
    ShowWindow(m_hwnd, SW_SHOWNORMAL);

    m_rect_origin = frameGeometry();

    m_is_full_screen = true;
    SetWindowLongPtrW(m_hwnd, GWL_STYLE, GetWindowLongPtrW(m_hwnd, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
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

    SetWindowTextW(m_hwnd, reinterpret_cast<WCHAR*>(const_cast<QChar*>(title.unicode())));

    QWidget::setWindowTitle(title);
#else
    return QMainWindow::setWindowTitle(title);
#endif
}

void QGoodWindow::setWindowIcon(const QIcon &icon)
{
#ifdef Q_OS_WIN
    if (!m_hwnd)
        return;

    HICON hicon_big = QtWin::toHICON(icon.pixmap(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON)));

    HICON hicon_small = QtWin::toHICON(icon.pixmap(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON)));

    SendMessageW(m_hwnd, WM_SETICON, ICON_BIG, LPARAM(hicon_big));
    SendMessageW(m_hwnd, WM_SETICON, ICON_SMALL, LPARAM(hicon_small));

    QWidget::setWindowIcon(QIcon(icon));
#else
    return QMainWindow::setWindowIcon(icon);
#endif
}

#ifdef Q_OS_WIN
LRESULT QGoodWindow::ncHitTest(int x, int y)
{
    int borderWidth = 1; //in pixels
    int titleHeight = titleBarHeight(); //in pixels

    // Get the point coordinates for the hit test.
    QPoint ptMouse = {x, y};

    // Get the window rectangle.
    QRect rcWindow = frameGeometry();

    // Determine if the hit test is for resizing. Default middle (1,1).
    int row = 1;
    int col = 1;
    bool onResizeBorder = false;

    bool maximized = isMaximized();

    if (maximized)
        titleHeight += GetSystemMetrics(SM_CYSIZEFRAME);

    // Determine if the point is at the top or bottom of the window.
    if (ptMouse.y() < rcWindow.top() + titleHeight)
    {
        onResizeBorder = (ptMouse.y() < (rcWindow.top() + borderWidth));
        row = 0;
    }
    else if (ptMouse.y() >= rcWindow.bottom() - borderWidth)
    {
        row = 2;
    }

    // Determine if the point is at the left or right of the window.
    if (ptMouse.x() < rcWindow.left() + borderWidth)
    {
        col = 0; // left side
    }
    else if (ptMouse.x() >= rcWindow.right() - borderWidth)
    {
        col = 2; // right side
    }
    else if (row == 0 && !onResizeBorder)
    {
        if (!maximized)
        {
            if (ptMouse.x() >= rcWindow.right() - rightMargin())
                return HTNOWHERE; // title bar buttons right
            else if (ptMouse.x() > rcWindow.left() + iconWidth() &&
                     ptMouse.x() < rcWindow.left() + iconWidth() + leftMargin())
                return HTNOWHERE; // custom title bar buttons left
            else if (ptMouse.x() < rcWindow.left() + iconWidth())
                return HTSYSMENU; // title bar icon
        }
        else
        {
            if (ptMouse.x() >= rcWindow.right() - BORDERWIDTH - rightMargin())
                return HTNOWHERE; // maximized title bar buttons right
            else if (ptMouse.x() > rcWindow.left() + BORDERWIDTH + iconWidth() &&
                     ptMouse.x() < rcWindow.left() + BORDERWIDTH + iconWidth() + leftMargin())
                return HTNOWHERE; // maximized custom title bar buttons left
            else if (ptMouse.x() < rcWindow.left() + BORDERWIDTH + iconWidth())
                return HTSYSMENU; // maximized title bar icon
        }
    }

    // Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
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

    MENUITEMINFOW mii;
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fType = 0;

    mii.fState = MF_ENABLED;
    SetMenuItemInfoW(hMenu, SC_RESTORE, FALSE, &mii);
    SetMenuItemInfoW(hMenu, SC_SIZE, FALSE, &mii);
    SetMenuItemInfoW(hMenu, SC_MOVE, FALSE, &mii);
    SetMenuItemInfoW(hMenu, SC_MAXIMIZE, FALSE, &mii);
    SetMenuItemInfoW(hMenu, SC_MINIMIZE, FALSE, &mii);

    mii.fState = MF_GRAYED;

    WINDOWPLACEMENT wp;
    GetWindowPlacement(m_hwnd, &wp);

    switch (wp.showCmd)
    {
    case SW_SHOWMAXIMIZED:
        SetMenuItemInfoW(hMenu, SC_SIZE, FALSE, &mii);
        SetMenuItemInfoW(hMenu, SC_MOVE, FALSE, &mii);
        SetMenuItemInfoW(hMenu, SC_MAXIMIZE, FALSE, &mii);
        SetMenuDefaultItem(hMenu, SC_CLOSE, FALSE);
        break;
    case SW_SHOWMINIMIZED:
        SetMenuItemInfoW(hMenu, SC_MINIMIZE, FALSE, &mii);
        SetMenuDefaultItem(hMenu, SC_RESTORE, FALSE);
        break;
    case SW_SHOWNORMAL:
        SetMenuItemInfoW(hMenu, SC_RESTORE, FALSE, &mii);
        SetMenuDefaultItem(hMenu, SC_CLOSE, FALSE);
        break;
    }

    if (!(GetWindowLongPtrW(m_hwnd, GWL_STYLE) & WS_MAXIMIZEBOX))
    {
        SetMenuItemInfoW(hMenu, SC_MAXIMIZE, FALSE, &mii);
        SetMenuItemInfoW(hMenu, SC_RESTORE, FALSE, &mii);
    }

    if (!(GetWindowLongPtrW(m_hwnd, GWL_STYLE) & WS_MINIMIZEBOX))
    {
        SetMenuItemInfoW(hMenu, SC_MINIMIZE, FALSE, &mii);
    }

    LPARAM cmd = TrackPopupMenu(hMenu, (TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD), x, y, 0, m_hwnd, nullptr);

    if (cmd)
        PostMessageW(m_hwnd, WM_SYSCOMMAND, WPARAM(cmd), 0);
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

void QGoodWindow::setClearColor(const QColor &color)
{
#ifdef Q_OS_WIN
    m_clear_color = color;
#else
    Q_UNUSED(color)
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

        if (QDialog *dialog = qobject_cast<QDialog*>(child_event->child()))
        {
            dialog->installEventFilter(this);
        }

        break;
    }
    case QEvent::WindowBlocked:
    {
        EnableWindow(HWND(winId()), FALSE);

        break;
    }
    case QEvent::WindowUnblocked:
    {
        EnableWindow(HWND(winId()), TRUE);

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
        for (Shadow *shadow : m_shadow_list)
            shadow->hide();

        break;
    }
    case QEvent::WindowStateChange:
    {
        if (!isVisible() || isMinimized() || isMaximized() || isFullScreen())
        {
            for (Shadow *shadow : m_shadow_list)
                shadow->hide();
        }

        break;
    }
    case QEvent::Resize:
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
            moveDialog(dialog, this);
        }

        break;
    }
    case QEvent::ShowToParent:
    {
        if (qobject_cast<QDialog*>(watched))
            m_dialog_visible_count++;

        break;
    }
    case QEvent::HideToParent:
    {
        if (qobject_cast<QDialog*>(watched))
            m_dialog_visible_count--;

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
            return (watched != this);

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

        if (m_margin == TITLE_BAR)
            return (watched != this);

        break;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

        if (m_resize_move && mouse_event->buttons() == Qt::LeftButton)
            sizeMove();
        else
            setCursorForCurrentPos();

        if (m_margin == TITLE_BAR)
            return (watched != this);

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

        if (m_margin == TITLE_BAR)
            return (watched != this);

        break;
    }
    case QEvent::Wheel:
    {
        if (m_margin == TITLE_BAR)
            return (watched != this);

        break;
    }
    case QEvent::ContextMenu:
    {
        if (m_margin == TITLE_BAR)
            return (watched != this);

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
            return (watched != this);

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

        if (margin == TITLE_BAR)
            return (watched != this);

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

        if (margin == TITLE_BAR)
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        }
        else
        {
            QApplication::setOverrideCursor(qobject_cast<QWidget*>(watched)->cursor());
        }

        if (margin == TITLE_BAR)
            return (watched != this);

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

        if (margin == TITLE_BAR)
            return (watched != this);

        break;
    }
    case QEvent::Wheel:
    {
        if (margin == TITLE_BAR)
            return (watched != this);

        break;
    }
    case QEvent::ContextMenu:
    {
        if (margin == TITLE_BAR)
            return (watched != this);

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

                //Fix mouse problems after resize or move
                QTest::mouseClick(windowHandle(), Qt::NoButton, Qt::NoModifier);
            }
            else
            {
                //Fix no mouse event after moving mouse from resize borders
                QEvent event(QEvent::MouseMove);
                QApplication::sendEvent(this, &event);
            }
        }
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}

#ifdef Q_OS_LINUX
void QGoodWindow::setCursorForCurrentPos()
{
    if (m_cursor_list.size() != SHADOW_WINDOW_COUNT)
        return;

    const QPoint cursor_pos = QCursor::pos();
    const int margin = ncHitTest(cursor_pos.x(), cursor_pos.y());

    m_cursor_pos = cursor_pos;
    m_margin = margin;

    QCursor cursor;

    switch (margin)
    {
    case TOP_LEFT:
    {
        if (!m_fixed_size)
            cursor = QCursor(m_cursor_list.at(0));
        else
            cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    case TOP:
    {
        if (!m_fixed_size)
            cursor = QCursor(m_cursor_list.at(1));
        else
            cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    case TOP_RIGHT:
    {
        if (!m_fixed_size)
            cursor = QCursor(m_cursor_list.at(2));
        else
            cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    case LEFT:
    {
        if (!m_fixed_size)
            cursor = QCursor(m_cursor_list.at(3));
        else
            cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    case RIGHT:
    {
        if (!m_fixed_size)
            cursor = QCursor(m_cursor_list.at(4));
        else
            cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    case BOTTOM_LEFT:
    {
        if (!m_fixed_size)
            cursor = QCursor(m_cursor_list.at(5));
        else
            cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    case BOTTOM:
    {
        if (!m_fixed_size)
            cursor = QCursor(m_cursor_list.at(6));
        else
            cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    case BOTTOM_RIGHT:
    {
        if (!m_fixed_size)
            cursor = QCursor(m_cursor_list.at(7));
        else
            cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    case TITLE_BAR:
    {
        cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    case NO_WHERE:
    {
        if (QWidget *widget = qApp->widgetAt(cursor_pos))
            cursor = widget->cursor();
        else
            cursor = QCursor(Qt::ArrowCursor);

        break;
    }
    default:
        break;
    }

    QApplication::setOverrideCursor(cursor);
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

    QRect geom = geometry();
    geom.adjust(-BORDERWIDTH, -BORDERWIDTH, BORDERWIDTH, BORDERWIDTH);

    const QRect available_geom = windowHandle()->screen()->availableGeometry();

    {   //TOP LEFT
        Shadow *shadow = m_shadow_list.at(0);

        QRect geom1 = geom;
        geom1.moveTo(geom1.left(), geom1.top());
        geom1.setWidth(BORDERWIDTH);
        geom1.setHeight(BORDERWIDTH);

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
        geom1.moveTo(geom1.right() - BORDERWIDTH + 1, geom1.top());
        geom1.setWidth(BORDERWIDTH);
        geom1.setHeight(BORDERWIDTH);

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
        geom1.moveTo(geom1.left(), geom1.bottom() - BORDERWIDTH + 1);
        geom1.setWidth(BORDERWIDTH);
        geom1.setHeight(BORDERWIDTH);

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
        geom1.moveTo(geom1.right() - BORDERWIDTH + 1, geom1.bottom() - BORDERWIDTH + 1);
        geom1.setWidth(BORDERWIDTH);
        geom1.setHeight(BORDERWIDTH);

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
        geom1.moveTo(geom1.left() + BORDERWIDTH, geom1.top());
        geom1.setWidth(geom1.width() - BORDERWIDTH * 2);
        geom1.setHeight(BORDERWIDTH);

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
        geom1.moveTo(geom1.left(), geom1.top() + BORDERWIDTH);
        geom1.setWidth(BORDERWIDTH);
        geom1.setHeight(geom1.height() - BORDERWIDTH * 2);

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
        geom1.moveTo(geom1.right() - BORDERWIDTH + 1, geom1.top() + BORDERWIDTH);
        geom1.setWidth(BORDERWIDTH);
        geom1.setHeight(geom1.height() - BORDERWIDTH * 2);

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
        geom1.moveTo(geom1.left() + BORDERWIDTH, geom1.bottom() - BORDERWIDTH + 1);
        geom1.setWidth(geom1.width() - BORDERWIDTH * 2);
        geom1.setHeight(BORDERWIDTH);

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
#if defined Q_OS_LINUX || defined Q_OS_MAC
int QGoodWindow::ncHitTest(int x, int y)
{
    const int titleHeight = titleBarHeight(); //in pixels

    // Get the point coordinates for the hit test.
    QPoint ptMouse = QPoint(x, y);

    // Get the window rectangle.
    QRect rcWindow = geometry();

    // Determine if the hit test is for resizing. Default middle (1,1).
    int row = 1;
    int col = 1;
    bool onResizeBorder = false;

    bool maximized = isMaximized();

    // Determine if the point is at the top or bottom of the window.
    if (ptMouse.y() < rcWindow.top() + titleHeight)
    {
        onResizeBorder = (ptMouse.y() < rcWindow.top());
        row = 0;
    }
    else if (!maximized && ptMouse.y() > rcWindow.bottom())
    {
        row = 2;
    }

    // Determine if the point is at the left or right of the window.
    if (!maximized && ptMouse.x() < rcWindow.left())
    {
        col = 0; // left side
    }
    else if (!maximized && ptMouse.x() > rcWindow.right())
    {
        col = 2; // right side
    }
    else if (row == 0 && !onResizeBorder)
    {
        if (!maximized)
        {
            if (ptMouse.x() > rcWindow.right() - rightMargin())
                return NO_WHERE; // custom title bar buttons right
            else if (ptMouse.x() > rcWindow.left() + iconWidth() &&
                     ptMouse.x() < rcWindow.left() + iconWidth() + leftMargin())
                return NO_WHERE; // custom title bar buttons left
        }
        else
        {
            if (ptMouse.x() > rcWindow.right() - rightMargin())
                return NO_WHERE; // maximized title bar buttons right
            else if (ptMouse.x() > rcWindow.left() + iconWidth() &&
                     ptMouse.x() < rcWindow.left() + iconWidth() + leftMargin())
                return NO_WHERE; // maximized custom title bar buttons left
        }
    }

    // Hit test (TOP_LEFT, ... BOTTOM_RIGHT)
    int hitTests[3][3] =
    {
        {TOP_LEFT, onResizeBorder ? TOP : TITLE_BAR, TOP_RIGHT},
        {LEFT, NO_WHERE, RIGHT},
        {BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT},
    };

    return hitTests[row][col];
}
#endif
