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
class SHAREDLIBSHARED_EXPORT QGoodWindow : public QMainWindow
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
    static QGoodStateHolder *getGoodStateHolder();

    /*** QGOODWINDOW FUNCTIONS END ***/

signals:
    /** On handled caption buttons, this SIGNAL report the state of these buttons. */
    void captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state);

    /** Notify that the system has changed between light and dark mode. */
    void systemThemeChanged();

public slots:
    /*** QGOODWINDOW FUNCTIONS BEGIN ***/

    /** Call setMargins() but only changes title bar height. */
    void setTitleBarHeight(int height);

    /** Call setMargins() but only changes icon width. */
    void setIconWidth(int width);

    /** Call setMargins() but only changes left margin. */
    void setLeftMargin(int width);

    /** Call setMargins() but only changes right margin. */
    void setRightMargin(int width);

    /** On Windows, Linux and macOS, returns the actual title bar height, on other OSes returns 0. */
    int titleBarHeight() const;

    /** On Windows, Linux and macOS, return the actual icon width, on other OSes returns 0. */
    int iconWidth() const;

    /** On Windows, Linux and macOS, returns the left margin of the customized title bar, on other OSes returns 0. */
    int leftMargin() const;

    /** On Windows, Linux and macOS, returns the right margin of the customized title bar, on other OSes returns 0. */
    int rightMargin() const;

    /** Set the tile bar height, icon width, left and right margins of the customized title bar.
        Note: Any call to setMargins() clear the left and right masks and the title bar caption buttons masks. */
    void setMargins(int title_bar_height, int icon_width, int left, int right);

    /** Set the mask for the left margin of the customized title bar. */
    void setLeftMask(const QRegion &mask);

    /** Set the mask for the right margin of the customized title bar. */
    void setRightMask(const QRegion &mask);

    /** Set the mask for the center of the customized title bar. */
    void setCenterMask(const QRegion &mask);

    /** Set the mask for the customized title bar, used in combination with others masks. */
    void setTitleBarMask(const QRegion &mask);

    /** Set if the caption buttons should be handled by *QGoodWindow* and on which \e corner,
     *valid only top left and top right corners. */
    void setCaptionButtonsHandled(bool handled, const Qt::Corner &corner = Qt::TopRightCorner);

    /** Set the location and shape of handled minimize button, relative to handled corner. */
    void setMinimizeMask(const QRegion &mask);

    /** Set the location and shape of handled maximize button, relative to handled corner. */
    void setMaximizeMask(const QRegion &mask);

    /** Set the location and shape of handled close button, relative to handled corner. */
    void setCloseMask(const QRegion &mask);

    /** Get the mask for the left margin of the customized title bar. */
    QRegion leftMask() const;

    /** Get the mask for the right margin of the customized title bar. */
    QRegion rightMask() const;

    /** Get the mask for the center of the customized title bar. */
    QRegion centerMask() const;

    /** Get the mask for the customized title bar. */
    QRegion titleBarMask() const;

    /** Get the location and shape of handled minimize button, relative to handled corner. */
    QRegion minimizeMask() const;

    /** Get the location and shape of handled maximize button, relative to handled corner. */
    QRegion maximizeMask() const;

    /** Get the location and shape of handled close button, relative to handled corner. */
    QRegion closeMask() const;

    /** Get the size that should be the size of the mask on the left margin of the customized title bar. */
    QSize leftMaskSize() const;

    /** Get the size that should be the size of the mask on the right margin of the customized title bar. */
    QSize rightMaskSize() const;

    /** If caption buttons are handled on left corner, their buttons masks should be in the bounds of this rect. */
    QRect leftCaptionButtonsRect() const;

    /** If caption buttons are handled on right corner, their buttons masks should be in the bounds of this rect. */
    QRect rightCaptionButtonsRect() const;

    /*** QGOODWINDOW FUNCTIONS END ***/

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

    /** Close the *QGoodWindow*. */
    void close();

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

    /** Returns a copy of the window state to restore it later. */
    QByteArray saveGeometry() const;

    /** Restore window to previous state \e geometry. */
    bool restoreGeometry(const QByteArray &geometry);

protected:
    //\cond HIDDEN_SYMBOLS
    //Functions
    bool event(QEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

#ifdef QT_VERSION_QT5
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif
#ifdef QT_VERSION_QT6
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#endif

private:
#ifdef QGOODWINDOW
#ifdef Q_OS_WIN
    //Functions
    void initGW();
    void destroyGW();
    static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void handleActivation();
    void handleDeactivation();
    void setWidgetFocus();
    void enableCaption();
    void disableCaption();
    void frameChanged();
    void sizeMoveWindow();
    void showContextMenu(int x, int y);
    void showContextMenu();
    QWidget *bestParentForModalWindow();
    void moveCenterWindow(QWidget *widget);
    bool winButtonHover(qintptr button);
    void iconClicked();

    //Variables
    HWND m_hwnd;
    bool m_is_win_11_or_greater;
    bool m_win_use_native_borders;
    QPointer<QMainWindow> m_main_window;
    QPointer<Shadow> m_shadow;
    QPointer<QWidget> m_helper_widget;
    QPointer<QWindow> m_helper_window;
    QGoodWindowUtils::NativeEventFilter *m_native_event;
    QWindow *m_window_handle;

    QPointer<QWidget> m_focus_widget;

    bool m_closed;

    QPointer<QTimer> m_timer_menu;
    bool m_is_menu_visible;

    bool m_is_full_screen;
    QRect m_rect_normal;

    bool m_active_state;

    Qt::WindowState m_state;

    QColor m_clear_color;

    friend class QGoodWindowUtils::NativeEventFilter;
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
    qreal m_pixel_ratio;
    Qt::WindowFlags m_window_flags;

    friend class Shadow;
#endif
#ifdef Q_OS_MAC
    //Functions
    void notificationReceiver(const QByteArray &notification);

    //Variables
    qreal m_pixel_ratio;
    QPoint m_pos;
    bool m_mouse_button_pressed;
    bool m_on_animate_event;

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
    qintptr ncHitTest(int x, int y);

    void buttonEnter(qintptr button);
    void buttonLeave(qintptr button);
    bool buttonPress(qintptr button);
    bool buttonRelease(qintptr button, bool valid_click);

    //Variables
    QPointer<QWidget> m_parent;

    QPointer<QTimer> m_hover_timer;

    QRegion m_left_mask;
    QRegion m_right_mask;
    QRegion m_center_mask;
    QRegion m_title_bar_mask;

    QRegion m_min_mask;
    QRegion m_max_mask;
    QRegion m_cls_mask;

    bool m_dark;

    bool m_caption_buttons_handled;
    Qt::Corner m_caption_buttons_corner;

    int m_title_bar_height;
    int m_icon_width;
    int m_left_margin;
    int m_right_margin;

    bool m_is_caption_button_pressed;
    qintptr m_last_caption_button_hovered;
    qintptr m_caption_button_pressed;
#endif
    //\endcond
};

#endif // QGOODWINDOW_H
