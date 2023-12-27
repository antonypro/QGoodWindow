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

#ifndef QGOODWINDOW_H
#define QGOODWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "intcommon.h"
#include "qgoodstateholder.h"
#include "lightstyle.h"
#include "darkstyle.h"

#include "qgoodwindow_global.h"

#ifdef QGOODWINDOW

#ifdef Q_OS_WIN
namespace QGoodWindowUtils
{
class NativeEventFilter;
}
#endif

#if defined Q_OS_WIN || defined Q_OS_LINUX
class Shadow;
#endif

#endif

/** **QGoodWindow** class contains the public API's to control the behavior of the customized window. */
class QGOODWINDOW_SHARED_EXPORT QGoodWindow : public QMainWindow
{
    Q_OBJECT
public:
    /** Constructor of *QGoodWindow*.
    *
    * On Windows creates the native window, turns the `QMainWindow` as a native widget,
    * creates the shadow, initialize default values and calls the `QMainWindow`
    * parent constructor.
    *
    * On Linux creates the frame less `QMainWindow`, use the shadow only to create resize borders,
    * and the real shadow is draw by current Linux window manager.
    *
    * On macOS creates a `QMainWindow` with full access to the title bar,
    * and hide native minimize, zoom and close buttons.
    */
    explicit QGoodWindow(QWidget *parent = nullptr,
                         const QColor &clear_color =
            QColor(!isSystemThemeDark() ? Qt::white : Qt::black));

    /** Destructor of *QGoodWindow*. */
    ~QGoodWindow();

    /** Enum that contains caption buttons states when it's states are handled by *QGoodWindow*. */
    enum class CaptionButtonState
    {
        /** Minimize button hover enter. */
        MinimizeHoverEnter,

        /** Minimize button hover leave. */
        MinimizeHoverLeave,

        /** Minimize button press. */
        MinimizePress,

        /** Minimize button release. */
        MinimizeRelease,

        /** Minimize button clicked. */
        MinimizeClicked,

        /** Maximize or restore button hover enter. */
        MaximizeHoverEnter,

        /** Maximize or restore button hover leave. */
        MaximizeHoverLeave,

        /** Maximize or restore button press. */
        MaximizePress,

        /** Maximize or restore button release. */
        MaximizeRelease,

        /** Maximize or restore button clicked. */
        MaximizeClicked,

        /** Close button hover enter. */
        CloseHoverEnter,

        /** Close button hover leave. */
        CloseHoverLeave,

        /** Close button press. */
        ClosePress,

        /** Close button release. */
        CloseRelease,

        /** Close button clicked. */
        CloseClicked
    };

    //Functions
    //\cond HIDDEN_SYMBOLS
    void themeChanged();
    //\endcond

    /** Returns the window id of the *QGoodWindow*. */
    WId winId() const;

    /** *QGoodWindow* handles flags internally, use this function only when *QGoodWindow* is not enabled. */
    void setWindowFlags(Qt::WindowFlags type);

    /** Returns the window flags of the *QGoodWindow*. */
    Qt::WindowFlags windowFlags() const;

    /*** QGOODWINDOW FUNCTIONS BEGIN ***/

    /** Returns the *QGoodWindow* version. */
    static QString version();

    /** Call this function to setup *QApplication* for *QGoodWindow* usage. */
    static void setup();

    /** Returns if the current system theme is dark or not. */
    static bool isSystemThemeDark();

    /** Returns true if system draw borders and false if your app should do it. */
    static bool shouldBordersBeDrawnBySystem();

    /** Show modal frame less *QDialog*, inside window \e child_gw, with parent \e parent_gw. */
    static int execDialog(QDialog *dialog, QGoodWindow *child_gw, QGoodWindow *parent_gw);

    /** Set the app theme to the dark theme. */
    static void setAppDarkTheme();

    /** Set the app theme to the light theme. */
    static void setAppLightTheme();

    /** Get the global state holder. */
    static QGoodStateHolder *qGoodStateHolderInstance();

    /*** QGOODWINDOW FUNCTIONS END ***/
Q_SIGNALS:
    /** On handled caption buttons, this SIGNAL report the state of these buttons. */
    void captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state);

    /** Notify that the system has changed between light and dark mode. */
    void systemThemeChanged();

    /** Notify that the visibility of caption buttons have changed on macOS. */
    void captionButtonsVisibilityChangedOnMacOS();

    /*** QGOODWINDOW FUNCTIONS BEGIN ***/

public:
    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED void setLeftMargin(int width)
    {Q_UNUSED(width); Q_ASSERT(false);}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED void setRightMargin(int width)
    {Q_UNUSED(width); Q_ASSERT(false);}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED int leftMargin() const
    {Q_ASSERT(false); return 0;}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED int rightMargin() const
    {Q_ASSERT(false); return 0;}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED void setMargins(int title_bar_height, int icon_width, int left, int right)
    {Q_UNUSED(title_bar_height); Q_UNUSED(icon_width); Q_UNUSED(left); Q_UNUSED(right); Q_ASSERT(false);}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED void setLeftMask(const QRegion &mask)
    {Q_UNUSED(mask); Q_ASSERT(false);}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED void setRightMask(const QRegion &mask)
    {Q_UNUSED(mask); Q_ASSERT(false);}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED void setCenterMask(const QRegion &mask)
    {Q_UNUSED(mask); Q_ASSERT(false);}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED void setCaptionButtonsHandled(bool handled, const Qt::Corner &corner = Qt::TopRightCorner)
    {Q_UNUSED(handled); Q_UNUSED(corner); Q_ASSERT(false);}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED QRegion leftMask() const
    {Q_ASSERT(false); return QRegion();}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED QRegion rightMask() const
    {Q_ASSERT(false); return QRegion();}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED QRegion centerMask() const
    {Q_ASSERT(false); return QRegion();}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED QSize leftMaskSize() const
    {Q_ASSERT(false); return QSize();}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED QSize rightMaskSize() const
    {Q_ASSERT(false); return QSize();}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED QRect leftCaptionButtonsRect() const
    {Q_ASSERT(false); return QRect();}

    /** Deprecated function scheduled for removal in some future version, don’t use it. */
    Q_DECL_DEPRECATED QRect rightCaptionButtonsRect() const
    {Q_ASSERT(false); return QRect();}

public Q_SLOTS:
    /** Set native caption buttons on macOS visibility to \e visible. **/
    void setNativeCaptionButtonsVisibleOnMac(bool visible);

    /** Returns if native caption buttons are visible on macOS. **/
    bool isNativeCaptionButtonsVisibleOnMac() const;

    /** Returns native caption buttons rect on macOS. **/
    QRect titleBarButtonsRectOnMacOS() const;

    /** Set title bar height for *QGoodWindow*. */
    void setTitleBarHeight(int height);

    /** Set current icon width on the left side of the title bar of *QGoodWindow*. */
    void setIconWidth(int width);

    /** On Windows, Linux and macOS, returns the actual title bar height, on other OSes returns 0. */
    int titleBarHeight() const;

    /** On Windows, Linux and macOS, return the actual icon width, on other OSes returns 0. */
    int iconWidth() const;

    /** Rect that contains the whole title bar. */
    QRect titleBarRect() const;

    /** Set the mask for the customized title bar. */
    void setTitleBarMask(const QRegion &mask);

    /** Set the location and shape of handled minimize button, relative to title bar rect. */
    void setMinimizeMask(const QRegion &mask);

    /** Set the location and shape of handled maximize button, relative to title bar rect. */
    void setMaximizeMask(const QRegion &mask);

    /** Set the location and shape of handled close button, relative to title bar rect. */
    void setCloseMask(const QRegion &mask);

    /** Get the mask for the customized title bar. */
    QRegion titleBarMask() const;

    /** Get the location and shape of handled minimize button, relative to title bar rect. */
    QRegion minimizeMask() const;

    /** Get the location and shape of handled maximize button, relative to title bar rect. */
    QRegion maximizeMask() const;

    /** Get the location and shape of handled close button, relative to title bar rect. */
    QRegion closeMask() const;

    /*** QGOODWINDOW FUNCTIONS END ***/

    /** Set central widget of *QGoodWindow* to \e widget. */
    void setCentralWidget(QWidget *widget);

    /** Returns the central widget of *QGoodWindow*. */
    QWidget *centralWidget() const;

    /** Get the size hint of *QGoodWindow*. */
    QSize sizeHint() const override;

    /** Get the minimum size hint of *QGoodWindow*. */
    QSize minimumSizeHint() const override;

    /** Set fixed size for *QGoodWindow* to width \e w and height \e h. */
    void setFixedSize(int w, int h);

    /** Set fixed size for *QGoodWindow* to \e size. */
    void setFixedSize(const QSize &size);

    /** Set minimum size for *QGoodWindow* to \e size. */
    void setMinimumSize(const QSize &size);

    /** Set maximum size for *QGoodWindow* to \e size. */
    void setMaximumSize(const QSize &size);

    /** Set minimum width for *QGoodWindow* to \e w. */
    void setMinimumWidth(int w);

    /** Set minimum height for *QGoodWindow* to \e h. */
    void setMinimumHeight(int h);

    /** Set maximum width for *QGoodWindow* to \e w. */
    void setMaximumWidth(int w);

    /** Set maximum height for *QGoodWindow* to \e h. */
    void setMaximumHeight(int h);

    /** Returns the *QGoodWindow* minimum size. */
    QSize minimumSize() const;

    /** Returns the *QGoodWindow* maximum size. */
    QSize maximumSize() const;

    /** Returns minimum width for *QGoodWindow* */
    int minimumWidth() const;

    /** Returns minimum height for *QGoodWindow* */
    int minimumHeight() const;

    /** Returns maximum width for *QGoodWindow* */
    int maximumWidth() const;

    /** Returns maximum height for *QGoodWindow* */
    int maximumHeight() const;

    /** Returns the geometry for *QGoodWindow* when it's state is no state. */
    QRect normalGeometry() const;

    /** Returns the geometry for *QGoodWindow* including extended frame and excluding shadow. */
    QRect frameGeometry() const;

    /** Returns the client area geometry. */
    QRect geometry() const;

    /** Returns the client area size, position is always `QPoint(0, 0)`. */
    QRect rect() const;

    /** Position of the window on screen. */
    QPoint pos() const;

    /** Size of the window on screen. */
    QSize size() const;

    /** X position of the window on screen. */
    int x() const;

    /** Y position of the window on screen. */
    int y() const;

    /** Width of the window. */
    int width() const;

    /** Height of the window. */
    int height() const;

    /** Move the window to \e x - \e y coordinates. */
    void move(int x, int y);

    /** Move the window to \e pos. */
    void move(const QPoint &pos);

    /** Resize the window to \e width - \e height size. */
    void resize(int width, int height);

    /** Resize the window to \e size. */
    void resize(const QSize &size);

    /** Set geometry to pos \e x - \e y, width \e w and height \e h. */
    void setGeometry(int x, int y, int w, int h);

    /** Set geometry to \e rect. */
    void setGeometry(const QRect &rect);

    /** Activates the *QGoodWindow*. */
    void activateWindow();

    /** Shows the *QGoodWindow*. */
    void show();

    /** Shows or restore the *QGoodWindow*. */
    void showNormal();

    /** Shows or maximize the *QGoodWindow*. */
    void showMaximized();

    /** Minimize the *QGoodWindow*. */
    void showMinimized();

    /** Turns the *QGoodWindow* into full screen mode. Including the title bar. */
    void showFullScreen();

    /** Hide the *QGoodWindow*. */
    void hide();

    /** Try to close the *QGoodWindow*, returns true if event is accepted or false otherwise. */
    bool close();

    /** Returns if the *QGoodWindow* is visible or not. */
    bool isVisible() const;

    /** Returns if the *QGoodWindow* is enabled or not. */
    bool isEnabled() const;

    /** Returns if the *QGoodWindow* is the foreground window or not. */
    bool isActiveWindow() const;

    /** Returns if the *QGoodWindow* is maximized or not. */
    bool isMaximized() const;

    /** Returns if the *QGoodWindow* is minimized or not. */
    bool isMinimized() const;

    /** Returns if the *QGoodWindow* is in full screen mode or not. */
    bool isFullScreen() const;

    /** Returns the *QGoodWindow* state. */
    Qt::WindowStates windowState() const;

    /** Sets the state of the *QGoodWindow* to \e state. */
    void setWindowState(Qt::WindowStates state);

    /** Returns the window handle of the *QGoodWindow*. */
    QWindow *windowHandle() const;

    /** Returns the opacity of the *QGoodWindow*. */
    qreal windowOpacity() const;

    /** Sets the opacity of the *QGoodWindow* to \e level. Where 0.0 is fully transparent and 1.0 fully opaque. */
    void setWindowOpacity(qreal level);

    /** Returns the title of the *QGoodWindow*. */
    QString windowTitle() const;

    /** Sets the title of the *QGoodWindow* to \e title. */
    void setWindowTitle(const QString &title);

    /** Returns the icon of the *QGoodWindow*. */
    QIcon windowIcon() const;

    /** Sets the icon of the *QGoodWindow* to \e icon. */
    void setWindowIcon(const QIcon &icon);

    /** Returns a copy of the *QGoodWindow* geometry to restore it later. */
    QByteArray saveGeometry() const;

    /** Restore *QGoodWindow* to previous geometry \e geometry. */
    bool restoreGeometry(const QByteArray &geometry);

protected:
    //\cond HIDDEN_SYMBOLS
    //Functions
    bool event(QEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

    bool nativeEvent(const QByteArray &eventType, void *message, qgoodintptr *result) override;

private:
#ifdef QGOODWINDOW
#ifdef Q_OS_WIN
    //Functions
    void initGW();
    void destroyGW();
    void closeGW();
    void setWindowStateWin();
    static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void handleActivation();
    void handleDeactivation();
    void setWidgetFocus();
    void enableCaption();
    void disableCaption();
    void disableMinimize();
    void disableMaximize();
    void frameChanged();
    void sizeMoveWindow();
    void sizeMoveMainWindow();
    void setWindowMask();
    void moveShadow();
    void screenChangeMoveWindow(QScreen *screen);
    void updateScreen(QScreen *screen);
    void setCurrentScreen(QScreen *screen);
    void windowScaleChanged();
    void internalWidgetResize();
    QPoint screenAdjustedPos();
    QScreen *screenForWindow(HWND hwnd);
    QScreen *screenForPoint(const QPoint &pos);
    void showContextMenu(int x, int y);
    void showContextMenu();
    QWidget *bestParentForModalWindow();
    void moveCenterWindow(QWidget *widget);
    bool winButtonHover(qintptr button);
    void iconClicked();

    //Variables
    HWND m_hwnd;
    bool m_is_win_11_or_greater;
    QPointer<QMainWindow> m_main_window;
    QPointer<QWidget> m_proxy_widget;
    QPointer<QWidget> m_central_widget;
    QPointer<Shadow> m_shadow;
    QPointer<QWidget> m_helper_widget;
#ifdef QT_VERSION_QT6
    QPointer<QWindow> m_helper_window;
#endif
    QGoodWindowUtils::NativeEventFilter *m_native_event;
    QWindow *m_window_handle;

    QPointer<QWidget> m_focus_widget;

    bool m_window_ready_for_resize;
    bool m_closed;
    bool m_visible;
    bool m_self_generated_close_event;
    QPointer<QTimer> m_timer_move;

    Qt::WindowStates m_window_state;

    QPointer<QTimer> m_timer_menu;
    bool m_is_menu_visible;

    QRect m_rect_normal;

    bool m_active_state;

    Qt::WindowState m_last_state;
    bool m_self_generated_show_event;

    QColor m_clear_color;

    friend class QGoodWindowUtils::NativeEventFilter;
    friend class QGoodDialog;
#endif
#ifdef Q_OS_LINUX
    //Functions
    void setCursorForCurrentPos();
    void startSystemMoveResize();
    void sizeMove();
    void sizeMoveBorders();

    //Variables
    QPointer<Shadow> m_shadow;

    int m_margin;
    QPoint m_cursor_pos;
    bool m_resize_move;
    bool m_resize_move_started;
    Qt::WindowFlags m_window_flags;
#endif
#ifdef Q_OS_MAC
    //Functions
    void notificationReceiver(const QByteArray &notification);
    void setMacOSStyle(int style_type);

    //Variables
    bool m_is_native_caption_buttons_visible_on_mac;
    QPoint m_cursor_move_pos;
    bool m_mouse_button_pressed;
    bool m_on_animate_event;
    void *style_ptr;

    friend class QGoodCentralWidget;
    friend class QGoodDialog;
    friend class Notification;
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
    int m_last_move_button;
#endif
#ifdef Q_OS_WIN
    int m_minimum_width;
    int m_minimum_height;
    int m_maximum_width;
    int m_maximum_height;
#endif
    //Functions
    qintptr ncHitTest(int pos_x, int pos_y);

    void buttonEnter(qintptr button);
    void buttonLeave(qintptr button);
    bool buttonPress(qintptr button);
    bool buttonRelease(qintptr button, bool valid_click);

    //Variables
    QPointer<QWidget> m_parent;

    QPointer<QTimer> m_hover_timer;

    QRegion m_title_bar_mask;

    QRegion m_min_mask;
    QRegion m_max_mask;
    QRegion m_cls_mask;

    qreal m_pixel_ratio;

    bool m_is_using_system_borders;

    bool m_dark;

    int m_title_bar_height;
    int m_icon_width;

    bool m_is_caption_button_pressed;
    qintptr m_last_caption_button_hovered;
    qintptr m_caption_button_pressed;
#endif
    //\endcond
};

#endif // QGOODWINDOW_H
