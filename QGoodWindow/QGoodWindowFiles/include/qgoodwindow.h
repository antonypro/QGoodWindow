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

#ifndef QGOODWINDOW_H
#define QGOODWINDOW_H

#include <QtCore>

#ifdef Q_OS_WIN
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0601
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#endif

#include <QtGui>
#include <QtWidgets>
#ifdef Q_OS_WIN
#include <QtWinExtras>
#endif

#ifdef Q_OS_WIN
class WinWidget;
class Shadow;
class ModalWidget;
#endif

/** **QGoodWindow** class contains the public API's to control the behavior of the customized window.
 *
 * **QGoodWindow** class inherits from `QMainWindow` which is used as a widget on Windows,
 * and in this case a native window is created, a `QWinWidget` fill this native window and finally the
 * *QGoodWindow* widget covers the `QWinWidget` using layouts.
 *
 * Any customization is done in *QGoodWindow* and the central widget added with the
 * `setCentralWidget` API inherited from `QMainWindow`.
 *
 * In other OSes than Windows the *QGoodWindow* behaves like original `QMainWindow`.
 */
class QGoodWindow : public QMainWindow
{
    Q_OBJECT
public:
    /** Constructor of *QGoodWindow*.
    *
    * On Windows creates the native window, the `QWinWidget`, turns the `QMainWindow` as
    * a widget, creates the shadow, initialize default values and calls the `QMainWindow`
    * parent constructor. On other OSes only calls the `QMainWindow` parent constructor.
    */
    explicit QGoodWindow(QWidget *parent = nullptr);

    /** Destructor of *QGoodWindow*. */
    ~QGoodWindow();

    //Functions

    /** Returns the window id of the native window. */
    WId winId() const;

    /** Returns the pixel ratio of the screen where the window is initialized.
    *
    * **Note:** It only returns a valid value on Windows, in all other OSes it returns 1.
    *
    * This function should be used only to scale items on Windows, on other OSes the pixel ratio **must** be
    * handled with Qt API's instead of this API.
    */
    qreal pixelRatio() const;

    /** Set the clear color of the native window. */
    static void setClearColor(const QColor &color);

public slots:
    /** Set fixed size for *QGoodWindow* to width \e w and height \e h. */
    void setFixedSize(int w, int h);

    /** Set fixed size for *QGoodWindow* to \e size. */
    void setFixedSize(const QSize &size);

    /** Returns the geometry of the native window on screen including extended frame and excluding shadow. */
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

    /** On Windows returns the actual title bar height, on other OSes returns 0. */
    int titleBarHeight() const;

    /** On Windows return the actual icon width, on other OSes returns 0. */
    int iconWidth() const;

    /** On Windows returns the left margin of the customized title bar, on other OSes returns 0. */
    int leftMargin() const;

    /** On Windows returns the right margin of the customized title bar, on other OSes returns 0. */
    int rightMargin() const;

    /** Set the tile bar height, icon width, left and right margins of the customized title bar. */
    void setMargins(int title_bar_height, int icon_width, int left, int right);


    /** Activates the native window. */
    void activateWindow();

    /** Shows the native window. */
    void show();

    /** Shows or restore the native window. */
    void showNormal();

    /** Shows or maximize the native window. */
    void showMaximized();

    /** Minimize the native window. */
    void showMinimized();

    /** Turns the native window into full screen mode. Including the title bar. */
    void showFullScreen();

    /** Hide the native window. */
    void hide();

    /** Close the native window. */
    void close();

    /** Returns if the native window is visible or not. */
    bool isVisible() const;

    /** Returns if the native window is enabled or not. */
    bool isEnabled() const;

    /** Returns if the native window is the foreground window or not. */
    bool isActiveWindow() const;

    /** Returns if the native window is maximized or not. */
    bool isMaximized() const;

    /** Returns if the native window is minimized or not. */
    bool isMinimized() const;

    /** Returns if the native window is in full screen mode or not. */
    bool isFullScreen() const;


    /** Returns the window handle of the native window. */
    QWindow *windowHandle() const;

    /** Returns the opacity the native window. */
    qreal windowOpacity() const;

    /** Sets the opacity of the native window to \e level. Where 0.0 is fully transparent and 1.0 fully opaque. */
    void setWindowOpacity(qreal level);

    /** Sets the title of the native window to \e title. */
    void setWindowTitle(const QString &title);

    /** Sets the icon of the native window to \e icon. */
    void setWindowIcon(const QPixmap &icon);

protected:
    //Functions
#ifdef Q_OS_WIN
    bool event(QEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
#endif

private:
    //Functions
#ifdef Q_OS_WIN
    static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void handleActivation();
    void handleDeactivation();
    void enableCaption();
    void disableCaption();
    void frameChanged();
    void sizeMoveWindow();
    LRESULT winNCHITTEST(int x, int y);
    void showContextMenu(int x, int y);
#endif

    //Variables
#ifdef Q_OS_WIN
    HWND m_hwnd;
    QPointer<QWidget> m_helper_widget;
    QPointer<WinWidget> m_win_widget;
    QPointer<Shadow> m_shadow;
    QPointer<QWidget> m_focus_widget;

    bool m_is_full_screen;
    QRect m_rect_origin;

    bool m_last_shadow_state_hidden;
    bool m_last_is_visible;
    bool m_last_minimized;
    bool m_last_maximized;
    bool m_last_fullscreen;
    bool m_last_visible;
    bool m_fixed_size;

    qreal m_pixel_ratio;

    QWindow *m_window_handle;
    int m_title_bar_height;
    int m_icon_width;
    int m_left_margin;
    int m_right_margin;
#endif
};

#endif // QGOODWINDOW_H
