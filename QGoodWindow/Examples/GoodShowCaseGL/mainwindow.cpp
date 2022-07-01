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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent, QColor(0, 38, 64))
{
    m_draw_borders = !QGoodWindow::shouldBordersBeDrawnBySystem();
    m_dark = QGoodWindow::isSystemThemeDark();

    m_frame_style = QString("QFrame {background-color: %0; border: %1;}");

    QString border_str = "none";

    GLWidget *gl = new GLWidget(this);

    QTimer *gl_timer = new QTimer(this);
    connect(gl_timer, &QTimer::timeout, gl, &GLWidget::rotate);
    gl_timer->setInterval(10);

#ifdef QGOODWINDOW
    title_bar = new TitleBar(pixelRatio(), this);

    frame = new QFrame(this);

    connect(this, &QGoodWindow::captionButtonStateChanged, title_bar, &TitleBar::captionButtonStateChanged);

    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(title_bar);
    layout->addWidget(gl);

    setCentralWidget(frame);

    connect(title_bar, &TitleBar::showMinimized, this, &MainWindow::showMinimized);
    connect(title_bar, &TitleBar::showNormal, this, &MainWindow::showNormal);
    connect(title_bar, &TitleBar::showMaximized, this, &MainWindow::showMaximized);
    connect(title_bar, &TitleBar::closeWindow, this, &MainWindow::close);

    connect(this, &MainWindow::windowTitleChanged, this, [=](const QString &title){
        title_bar->setTitle(title);
    });

    connect(this, &MainWindow::windowIconChanged, this, [=](const QIcon &icon){
        if (!icon.isNull())
        {
            const int pix_size = qCeil(16 * pixelRatio());
            title_bar->setIcon(icon.pixmap(pix_size, pix_size));
        }
    });

    if (m_draw_borders && windowState().testFlag(Qt::WindowNoState))
        border_str = "1px solid #1883D7";
#else
    frame = new QFrame(this);

    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(gl);

    setCentralWidget(frame);
#endif

    auto theme_change_func = [=]{
#ifdef QGOODWINDOW
        title_bar->setDarkMode(m_dark);
#endif

        if (!m_dark)
            m_color_str = "#FFFFFF";
        else
            m_color_str = "#000000";

        QString border_str = "none";

        if (m_draw_borders && windowState().testFlag(Qt::WindowNoState))
            border_str = "1px solid #1883D7";

        frame->setStyleSheet(m_frame_style.arg(m_color_str).arg(border_str));
    };

    connect(this, &QGoodWindow::systemThemeChanged, this, [=]{
        m_dark = QGoodWindow::isSystemThemeDark();
        theme_change_func();
    });

    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::Key_S), this);

    connect(shortcut1, &QShortcut::activated, this, [=]{
        m_dark = !m_dark;
        theme_change_func();
    });

    QShortcut *shortcut2 = new QShortcut(QKeySequence(Qt::Key_F), this);

    connect(shortcut2, &QShortcut::activated, this, [=]{
        if (!isFullScreen())
            showFullScreen();
        else
            showNormal();
    });

    theme_change_func();

    QPixmap p = QPixmap(1, 1);
    p.fill(Qt::red);

    setWindowIcon(p);

    setWindowTitle("Good Window - Press S to toggle theme - Press F to toggle fullscreen!");

    resize(qRound(640 * pixelRatio()), qRound(480 * pixelRatio()));

    move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());

#ifdef Q_OS_WIN
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTimer::singleShot(0, this, [=]{
        QPixmap p = QPixmap(1, 1);
        p.fill(Qt::yellow);

        QWinTaskbarButton *button = new QWinTaskbarButton(this);
        button->setWindow(windowHandle());
        button->setOverlayIcon(p);

        QWinTaskbarProgress *progress = button->progress();
        progress->setVisible(true);
        progress->setValue(50);
    });
#endif
#endif

    gl_timer->start();
}

MainWindow::~MainWindow()
{

}

#ifdef QGOODWINDOW
void MainWindow::styleWindow()
{
    bool window_active = isActiveWindow();
    bool window_no_state = windowState().testFlag(Qt::WindowNoState);
    bool draw_borders = m_draw_borders;
    bool is_maximized = isMaximized();
    bool is_full_screen = isFullScreen();
    qreal pixel_ratio = pixelRatio();

    QString border_str = "none";

    if (draw_borders && window_no_state)
    {
        if (isActiveWindow())
            border_str = "1px solid #1883D7";
        else
            border_str = "1px solid #AAAAAA";
    }

    frame->setStyleSheet(m_frame_style.arg(m_color_str).arg(border_str));

    title_bar->setMaximized(is_maximized);

    title_bar->setVisible(!is_full_screen);

    if (!is_full_screen)
    {
        setMargins(qRound(30 * pixel_ratio), qRound(30 * pixel_ratio), 0, qRound(36 * 3 * pixel_ratio));

        setCaptionButtonsHandled(true, Qt::TopRightCorner);

        QRect rect = rightCaptionButtonsRect();

        QRect min_rect = QRect(0, 0, rect.width(), rect.height());
        QRect max_rect = QRect((rect.width() / 3), 0, rect.width(), rect.height());
        QRect cls_rect = QRect((rect.width() / 3) * 2, 0, rect.width(), rect.height());

        setMinimizeMask(min_rect);
        setMaximizeMask(max_rect);
        setCloseMask(cls_rect);
    }
    else
    {
        setMargins(0, 0, 0, 0);
    }

    if (window_active)
    {
        if (draw_borders)
        {
            QString border_str = (windowState().testFlag(Qt::WindowNoState) ? "1px solid #1883D7" : "none");
            frame->setStyleSheet(m_frame_style.arg(m_color_str).arg(border_str));
        }

        title_bar->setActive(true);
    }
    else
    {
        if (draw_borders)
        {
            QString border_str = (windowState().testFlag(Qt::WindowNoState) ? "1px solid #AAAAAA" : "none");
            frame->setStyleSheet(m_frame_style.arg(m_color_str).arg(border_str));
        }

        title_bar->setActive(false);
    }
}
#endif

void MainWindow::closeEvent(QCloseEvent *event)
{
    int result = QMessageBox::question(this, "Close window", "Are you sure to close?");

    if (result != QMessageBox::Yes)
        event->ignore();
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
#else
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#endif
{
    //Listen for native events

    return QGoodWindow::nativeEvent(eventType, message, result);
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type())
    {
#ifdef QGOODWINDOW
    case QEvent::Show:
    case QEvent::WindowStateChange:
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
    {
        styleWindow();
        break;
    }
#endif
    case QEvent::Resize:
    case QEvent::Hide:
    case QEvent::Move:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseMove:
    case QEvent::Wheel:
        //Listen for Qt window events
        break;
    default:
        break;
    }

    return QGoodWindow::event(event);
}
