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

#ifndef QGOODWINDOW_H
#define QGOODWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

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

/** **QGoodWindow** class contains the public API's to control the behavior of the customized window.
 *
 * On Windows, **QGoodWindow** class inherits from `QMainWindow` which is used as a native widget that
 * creates, fill and control the native window of **QGoodWindow**.
 *
 * On Linux and macOS the **QGoodWindow** behaves like a frameless `QMainWindow`.
 */
class QGoodWindow : public QMainWindow
{
    Q_OBJECT
public:
    /** Constructor of *QGoodWindow*.
    *
    * On Windows creates the native window, turns the `QMainWindow` as a native widget,
    * creates the shadow, initialize default values and calls the `QMainWindow`
    * parent constructor.
    *
    * On Linux creates the frameless `QMainWindow`, use the shadow only to create resize borders,
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

    /** Enum that contains caption buttons states when it's states are handled by *QGoodWindow*.*/
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
    /** Returns the window id of the *QGoodWindow*. */
    WId winId() const;

    //Variables
    /** Reserved. */
    QPointer<QTimer> m_theme_change_timer;

signals:
    /** On handled caption buttons, this SIGNAL report the state of these buttons. */
    void captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state);

    /** Notify that the system has changed between light and dark mode. */
    void systemThemeChanged();

public slots:
    /*** QGOODWINDOW FUNCTIONS BEGIN ***/

    /** Returns if the current system theme is dark or not. */
    static bool isSystemThemeDark();

    /** Returns if there is a one pixel margin around window for resizing or not, i.e. if system draw margins. */
    static bool shouldBordersBeDrawnBySystem();

    /** On Windows, Linux and macOS, returns the actual title bar height, on other OSes returns 0. */
    int titleBarHeight() const;

    /** On Windows, Linux and macOS, return the actual icon width, on other OSes returns 0. */
    int iconWidth() const;

    /** On Windows, Linux and macOS, returns the left margin of the customized title bar, on other OSes returns 0. */
    int leftMargin() const;

    /** On Windows, Linux and macOS, returns the right margin of the customized title bar, on other OSes returns 0. */
    int rightMargin() const;

    /** Set the tile bar height, icon width, left and right margins of the customized title bar. */
    void setMargins(int title_bar_height, int icon_width, int left, int right);

    /** Set the mask for the left margin of the customized title bar. */
    void setLeftMask(const QRegion &mask);

    /** Set the mask for the right margin of the customized title bar. */
    void setRightMask(const QRegion &mask);

    /** Set if the caption buttons should be handled by *QGoodWindow* and on which \e corner, valid only top left and top right corners. */
    void setCaptionButtonsHandled(bool handled, const Qt::Corner &corner = Qt::TopRightCorner);

    /** Set the location and shape of handled minimize button, relative to handled corner. */
    void setMinimizeMask(const QRegion &mask);

    /** Set the location and shape of handled maximize button, relative to handled corner. */
    void setMaximizeMask(const QRegion &mask);

    /** Set the location and shape of handled close button, relative to handled corner. */
    void setCloseMask(const QRegion &mask);

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

protected:
    //Functions
    bool event(QEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#else
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result);
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
    LRESULT ncHitTest(int x, int y);
    void showContextMenu(int x, int y);
    void showContextMenu();
    QWidget *bestParentForModalWindow();
    void moveCenterWindow(QWidget *widget);
    bool winButtonHover(long button);

    //Variables
    HWND m_hwnd;
    bool m_win_use_native_borders;
    QPointer<QMainWindow> m_main_window;
    QPointer<Shadow> m_shadow;
    QPointer<QWidget> m_helper_widget;
    QGoodWindowUtils::NativeEventFilter *m_native_event;
    QWindow *m_window_handle;

    QPointer<QWidget> m_focus_widget;

    bool m_closed;

    bool m_is_full_screen;
    QRect m_rect_origin;

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
    QList<QPointer<Shadow>> m_shadow_list;

    int m_margin;
    QPoint m_cursor_pos;
    bool m_resize_move;
    bool m_resize_move_started;
    qreal m_pixel_ratio;

    friend class Shadow;
#endif
#ifdef Q_OS_MAC
    //Functions
    void notificationReceiver(const QByteArray &notification);

    //Variables
    QPoint m_pos;
    bool m_mouse_button_pressed;
    bool m_on_animate_event;

    friend class Notification;
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
    //Functions
    int ncHitTest(int x, int y);

    //Variables
    int m_last_move_button;
#endif
#if defined Q_OS_WIN || defined Q_OS_LINUX
    bool m_fixed_size;
#endif
#ifdef Q_OS_LINUX
    bool m_last_fixed_size_value;
#endif
    //Functions
    void buttonEnter(long button);
    void buttonLeave(long button);
    bool buttonPress(long button);
    bool buttonRelease(long button, bool valid_click);

    //Variables
    QPointer<QTimer> m_hover_timer;

    QRegion m_left_mask;
    QRegion m_right_mask;

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
    long m_last_caption_button_hovered;
    long m_caption_button_pressed;
#endif
};

#endif // QGOODWINDOW_H
