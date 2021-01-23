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

#ifndef QGOODWINDOW_H
#define QGOODWINDOW_H

#ifdef _WIN32

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT _WIN32_WINNT_VISTA

#endif

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#ifdef Q_OS_WIN
#include <QtWinExtras>
#endif

#ifdef Q_OS_WIN
class WinWidget;
#endif

class Shadow;

/** **QGoodWindow** class contains the public API's to control the behavior of the customized window.
 *
 * On Windows, **QGoodWindow** class inherits from `QMainWindow` which is used as a widget,
 * and in this case a native window is created, a `QWinWidget` fill this native window and finally the
 * *QGoodWindow* widget covers the `QWinWidget` using layouts.
 *
 * On Linux and macOS the **QGoodWindow** behaves like a frameless `QMainWindow`.
 */
class QGoodWindow : public QMainWindow
{
    Q_OBJECT
public:
    /** Constructor of *QGoodWindow*.
    *
    * On Windows creates the native window, the `QWinWidget`, turns the `QMainWindow` as
    * a widget, creates the shadow, initialize default values and calls the `QMainWindow`
    * parent constructor.
    *
    * On Linux creates the frameless `QMainWindow`, use the shadow only to create resize borders,
    * and the real shadow is draw by Linux window manager.
    *
    * On macOS creates a `QMainWindow` with full access to the title bar,
    * and hide native minimize, zoom and close buttons.
    */
    explicit QGoodWindow(QWidget *parent = nullptr);

    /** Destructor of *QGoodWindow*. */
    ~QGoodWindow();

    //Functions
    /** Returns the window id of the window. */
    WId winId() const;

    /** Set the clear color of the *QGoodWindow*. Valid only for Windows. */
    static void setClearColor(const QColor &color);

public slots:
    /** Set fixed size for *QGoodWindow* to width \e w and height \e h. */
    void setFixedSize(int w, int h);

    /** Set fixed size for *QGoodWindow* to \e size. */
    void setFixedSize(const QSize &size);

    /** Returns the geometry for *QGoodWindow* including extended frame and excluding shadow. */
    QRect frameGeometry() const;

    /** Same as *frameGeometry()*. */
    QRect geometry() const;

    /** Geometry excluding position, position is always `QPoint(0, 0)`. */
    QRect rect() const;

    /** Position of the window on screen. */
    QPoint pos() const;

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


    /** Returns the window handle of the *QGoodWindow*. */
    QWindow *windowHandle() const;

    /** Returns the opacity the *QGoodWindow*. */
    qreal windowOpacity() const;

    /** Sets the opacity of the *QGoodWindow* to \e level. Where 0.0 is fully transparent and 1.0 fully opaque. */
    void setWindowOpacity(qreal level);

    /** Sets the title of the *QGoodWindow* to \e title. */
    void setWindowTitle(const QString &title);

    /** Sets the icon of the *QGoodWindow* to \e icon. */
    void setWindowIcon(const QIcon &icon);

protected:
    //Functions
    bool event(QEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:
#ifdef QGOODWINDOW
#ifdef Q_OS_WIN
    //Functions
    static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void handleActivation();
    void handleDeactivation();
    void enableCaption();
    void disableCaption();
    void frameChanged();
    void sizeMoveWindow();
    LRESULT ncHitTest(int x, int y);
    void showContextMenu(int x, int y);

    //Variables
    QPointer<WinWidget> m_win_widget;
    QPointer<Shadow> m_shadow;
    HWND m_hwnd;
    QPointer<QWidget> m_helper_widget;
    QPointer<QWidget> m_focus_widget;

    bool m_is_full_screen;
    QRect m_rect_origin;

    bool m_last_shadow_state_hidden;
    bool m_last_is_visible;
    bool m_last_minimized;
    bool m_last_maximized;
    bool m_last_fullscreen;
    bool m_window_ready;

    qreal m_pixel_ratio;

    QWindow *m_window_handle;
    bool m_last_state_is_inactive;

    int m_dialog_visible_count;
#endif
#ifdef Q_OS_LINUX
    //Functions
    inline QPixmap openCursor(const QString &name);
    inline QString convertIndexToName(int index);
    void setCursorForCurrentPos();
    void startSystemMoveResize();
    void sizeMove();
    void sizeMoveBorders();

    //Variables
    QList<QPointer<Shadow>> m_shadow_list;

    QList<QPixmap> m_cursor_list;

    int m_margin;
    QPoint m_cursor_pos;
    bool m_resize_move;
    bool m_resize_move_started;

    friend class Shadow;
#endif
#ifdef Q_OS_MAC
    void notificationReceiver(const QByteArray &notification);
    QPoint m_pos;

    friend class Notification;
#endif
#if defined Q_OS_LINUX || defined Q_OS_MAC
    int ncHitTest(int x, int y);
#endif
#if defined Q_OS_WIN || defined Q_OS_LINUX
    bool m_fixed_size;
#endif

    int m_title_bar_height;
    int m_icon_width;
    int m_left_margin;
    int m_right_margin;
#endif
};

#endif // QGOODWINDOW_H
