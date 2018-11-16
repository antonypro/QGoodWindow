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

class QGoodWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit QGoodWindow(QWidget *parent = nullptr);
    ~QGoodWindow();

    //Functions
    WId winId();
    qreal pixelRatio();
    QWidget *parentForModal();
#ifdef Q_OS_WIN
    void setAttribute(Qt::WidgetAttribute attribute, bool on = true);
    bool testAttribute(Qt::WidgetAttribute attribute);
#endif

public slots:
    void setFixedSize(int w, int h);
    void setFixedSize(const QSize &size);
    QRect frameGeometry();
    QRect geometry();
    QRect rect();
    QPoint pos();
    int x();
    int y();
    int width();
    int height();
    void move(int x, int y);
    void move(const QPoint &pos);
    void resize(int width, int height);
    void resize(const QSize &size);
    void setGeometry(int x, int y, int w, int h);
    void setGeometry(const QRect &rect);

    int titleBarHeight();
    int iconWidth();
    int leftMargin();
    int rightMargin();
    void setMargins(int title_bar_height, int icon_width, int left, int right);

    void activateWindow();
    void show();
    void showNormal();
    void showMaximized();
    void showMinimized();
    void showFullScreen();
    void hide();
    void close();
    bool isVisible();
    bool isEnabled();
    bool isActiveWindow();
    bool isMaximized();
    bool isMinimized();
    bool isFullScreen();

    QWindow *windowHandle();

    qreal windowOpacity();
    void setWindowOpacity(qreal level);

    void setWindowTitle(const QString &title);
    void setWindowIcon(const QPixmap &icon);

private:
    //Functions
#ifdef Q_OS_WIN
    static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void handleActivation();
    void handleDeactivation();
    void enableCaption();
    void disableCaption();
    void sizeMoveWindow();
    LRESULT winNCHITTEST(int x, int y);
    void showContextMenu(int x, int y);
#endif

    //Variables
#ifdef Q_OS_WIN
    HWND m_hwnd;
    QWidget *m_helper_widget;
    WinWidget *m_win_widget;
    Shadow *m_shadow;

    bool m_is_full_screen;
    QRect m_rect_origin;

    bool m_last_shadow_state_hidden;
    bool m_last_minimized;
    bool m_last_maximized;
    bool m_last_fullscreen;
    bool m_fixed_size;
    QList<Qt::WidgetAttribute> m_attributes;

    qreal m_pixel_ratio;

    QWindow *m_window_handle;
    int m_title_bar_height;
    int m_icon_width;
    int m_left_margin;
    int m_right_margin;
#endif
};

#endif // QGOODWINDOW_H
