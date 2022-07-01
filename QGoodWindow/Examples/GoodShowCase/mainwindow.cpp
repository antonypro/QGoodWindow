/*
The MIT License (MIT)

Copyright © 2022 Antonio Dias

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

#ifdef _WIN32

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <windows.h>
#include <dwmapi.h>

#endif

#include "mainwindow.h"
#include "lightstyle.h"
#include "darkstyle.h"

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent)
{
#ifdef QGOODWINDOW
    // if the system will draw borders or if this application must do that
    m_draw_borders = !QGoodWindow::shouldBordersBeDrawnBySystem();

    m_dark = QGoodWindow::isSystemThemeDark();

    // create frameless window
    m_window = new FramelessWindow(this);
    m_central_widget = new CentralWidget(m_window);

    // add the mainwindow to our custom frameless window
    m_window->ui->windowContent->layout()->addWidget(m_central_widget);

    connect(this, &QGoodWindow::windowTitleChanged, this, [=](const QString &title){
        m_window->ui->titleWidget->setText(title);
    });

    connect(this, &QGoodWindow::windowIconChanged, this, [=](const QIcon &icon){
        m_window->ui->icon->setPixmap(icon.pixmap(16, 16));
    });

    m_window->ui->icon->setFixedSize(30, 20);

    m_window->ui->minimizeButton->setFixedSize(27, 27);
    m_window->ui->maximizeButton->setFixedSize(27, 27);
    m_window->ui->restoreButton->setFixedSize(27, 27);
    m_window->ui->closeButton->setFixedSize(27, 27);

    m_window->ui->minimizeButton->init(CaptionButton::IconType::Minimize);
    m_window->ui->maximizeButton->init(CaptionButton::IconType::Maximize);
    m_window->ui->restoreButton->init(CaptionButton::IconType::Restore);
    m_window->ui->closeButton->init(CaptionButton::IconType::Close);

    connect(m_window->ui->minimizeButton, &CaptionButton::clicked, this, &MainWindow::showMinimized);
    connect(m_window->ui->maximizeButton, &CaptionButton::clicked, this, &MainWindow::showMaximized);
    connect(m_window->ui->restoreButton, &CaptionButton::clicked, this, &MainWindow::showNormal);
    connect(m_window->ui->closeButton, &CaptionButton::clicked, this, &MainWindow::close);

    connect(this, &QGoodWindow::captionButtonStateChanged, this, &MainWindow::captionButtonStateChanged);

    setMargins(30, 30, 0, 27 * 3);

    setCaptionButtonsHandled(true, Qt::TopRightCorner);

    // Overlap close with maximize and maximize with minimize
    setCloseMask(QRect(27 * 2, 0, rightCaptionButtonsRect().width(), rightCaptionButtonsRect().height()));
    setMaximizeMask(QRect(27 * 1, 0, rightCaptionButtonsRect().width(), rightCaptionButtonsRect().height()));
    setMinimizeMask(QRect(0, 0, rightCaptionButtonsRect().width(), rightCaptionButtonsRect().height()));

    auto theme_change_func = [=]{
        if (m_dark)
            darkTheme();
        else
            lightTheme();

        //Icon color inverse of m_dark to contrast.
        m_window->ui->minimizeButton->setIconMode(!m_dark);
        m_window->ui->maximizeButton->setIconMode(!m_dark);
        m_window->ui->restoreButton->setIconMode(!m_dark);
        m_window->ui->closeButton->setIconMode(!m_dark);
    };

    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);

    connect(shortcut1, &QShortcut::activated, this, [=]{
        m_dark = !m_dark;
        theme_change_func();
    });

    connect(this, &QGoodWindow::systemThemeChanged, this, [=]{
        m_dark = QGoodWindow::isSystemThemeDark();
        theme_change_func();
    });

    theme_change_func();
#endif

    setWindowIcon(qApp->style()->standardIcon(QStyle::SP_DesktopIcon));
    setWindowTitle("Good Window - Press CTRL+S to toggle theme!");

#ifdef QGOODWINDOW
    resize(m_window->size());
    setCentralWidget(m_window);
#else
    m_dark = QGoodWindow::isSystemThemeDark();

    auto theme_change_func = [=]{
        if (m_dark)
            darkTheme();
        else
            lightTheme();
    };

    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);

    connect(shortcut1, &QShortcut::activated, this, [=]{
        m_dark = !m_dark;
        theme_change_func();
    });

    connect(this, &QGoodWindow::systemThemeChanged, this, [=]{
        m_dark = QGoodWindow::isSystemThemeDark();
        theme_change_func();
    });

    theme_change_func();

    m_central_widget = new CentralWidget(this);
    m_central_widget->setWindowFlags(Qt::Widget);
    resize(m_central_widget->size());
    setCentralWidget(m_central_widget);
#endif
    move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());
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

    if (window_active)
    {
        if (window_no_state)
        {
            if (draw_borders)
            {
                m_window->ui->windowTitlebar->setStyleSheet(
                            QStringLiteral("#windowTitlebar{border: 0px none palette(shadow);}"));
                m_window->ui->windowFrame->setStyleSheet(
                            QStringLiteral("#windowFrame{border: 1px solid palette(highlight);"
                                           "background-color: palette(Window);}"));
            }
            else
            {
                m_window->ui->windowTitlebar->setStyleSheet(
                            QStringLiteral("#windowTitlebar{border: 0px none palette(shadow);}"));
                m_window->ui->windowFrame->setStyleSheet(
                            QStringLiteral("#windowFrame{border: 0px solid palette(highlight);"
                                           "background-color: palette(Window);}"));
            }
        }
        else
        {
            m_window->ui->windowTitlebar->setStyleSheet(
                        QStringLiteral("#windowTitlebar{border: 0px none palette(shadow);"
                                       "background-color :palette(shadow); height:20px;}"));
            m_window->ui->windowFrame->setStyleSheet(
                        QStringLiteral("#windowFrame{border: 0px none palette(dark);"
                                       "background-color: palette(Window);}"));
        }
    }
    else
    {
        if (window_no_state)
        {
            if (draw_borders)
            {
                m_window->ui->windowTitlebar->setStyleSheet(
                            QStringLiteral("#windowTitlebar{border: 0px none palette(shadow);"
                                           "background-color: palette(dark); height:20px;}"));
                m_window->ui->windowFrame->setStyleSheet(
                            QStringLiteral("#windowFrame{border: 1px solid #000000;"
                                           "background-color: palette(Window);}"));
            }
            else
            {
                m_window->ui->windowTitlebar->setStyleSheet(
                            QStringLiteral("#windowTitlebar{border: 0px none palette(shadow);"
                                           "background-color: palette(dark); height:20px;}"));
                m_window->ui->windowFrame->setStyleSheet(
                            QStringLiteral("#windowFrame{border: 0px solid #000000;"
                                           "background-color: palette(Window);}"));
            }
        }
        else
        {
            m_window->ui->windowTitlebar->setStyleSheet(
                        QStringLiteral("#titlebarWidget{border: 0px none palette(shadow);"
                                       "background-color: palette(dark); height: 20px;}"));
            m_window->ui->windowFrame->setStyleSheet(
                        QStringLiteral("#windowFrame{border: 0px none palette(shadow);"
                                       "background-color: palette(Window);}"));
        }
    }

    m_window->ui->icon->setActive(window_active);

    m_window->ui->titleWidget->setActive(window_active);

    if (!isMinimized())
    {
        m_window->ui->maximizeButton->setVisible(window_no_state);
        m_window->ui->restoreButton->setVisible(!window_no_state);
    }

    m_window->ui->minimizeButton->setActive(window_active);
    m_window->ui->maximizeButton->setActive(window_active);
    m_window->ui->restoreButton->setActive(window_active);
    m_window->ui->closeButton->setActive(window_active);
}

void MainWindow::captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state)
{
    switch (state)
    {
        // Hover enter
    case QGoodWindow::CaptionButtonState::MinimizeHoverEnter:
    {
        m_window->ui->minimizeButton->setState(QEvent::HoverEnter);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeHoverEnter:
    {
        if (!isMaximized())
            m_window->ui->maximizeButton->setState(QEvent::HoverEnter);
        else
            m_window->ui->restoreButton->setState(QEvent::HoverEnter);

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseHoverEnter:
    {
        m_window->ui->closeButton->setState(QEvent::HoverEnter);

        break;
    }
        // Hover leave
    case QGoodWindow::CaptionButtonState::MinimizeHoverLeave:
    {
        m_window->ui->minimizeButton->setState(QEvent::HoverLeave);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeHoverLeave:
    {
        if (!isMaximized())
            m_window->ui->maximizeButton->setState(QEvent::HoverLeave);
        else
            m_window->ui->restoreButton->setState(QEvent::HoverLeave);

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseHoverLeave:
    {
        m_window->ui->closeButton->setState(QEvent::HoverLeave);

        break;
    }
        // Mouse button press
    case QGoodWindow::CaptionButtonState::MinimizePress:
    {
        m_window->ui->minimizeButton->setState(QEvent::MouseButtonPress);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizePress:
    {
        if (!isMaximized())
            m_window->ui->maximizeButton->setState(QEvent::MouseButtonPress);
        else
            m_window->ui->restoreButton->setState(QEvent::MouseButtonPress);

        break;
    }
    case QGoodWindow::CaptionButtonState::ClosePress:
    {
        m_window->ui->closeButton->setState(QEvent::MouseButtonPress);

        break;
    }
        // Mouse button release
    case QGoodWindow::CaptionButtonState::MinimizeRelease:
    {
        m_window->ui->minimizeButton->setState(QEvent::MouseButtonRelease);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeRelease:
    {
        if (!isMaximized())
            m_window->ui->maximizeButton->setState(QEvent::MouseButtonRelease);
        else
            m_window->ui->restoreButton->setState(QEvent::MouseButtonRelease);

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseRelease:
    {
        m_window->ui->closeButton->setState(QEvent::MouseButtonRelease);

        break;
    }
        // Mouse button clicked
    case QGoodWindow::CaptionButtonState::MinimizeClicked:
    {
        emit m_window->ui->minimizeButton->clicked();

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeClicked:
    {
        if (!isMaximized())
            emit m_window->ui->maximizeButton->clicked();
        else
            emit m_window->ui->restoreButton->clicked();

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseClicked:
    {
        emit m_window->ui->closeButton->clicked();

        break;
    }
    default:
        break;
    }
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Show:
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
    case QEvent::WindowStateChange:
    {
        styleWindow();
        break;
    }
    case QEvent::StyleChange:
    {
        QColor active_color = qApp->palette().color(QPalette::WindowText);
        QColor inactive_color = qApp->palette().color(QPalette::Disabled, QPalette::WindowText);

        m_window->ui->titleWidget->setTitleColor(active_color, inactive_color);

        break;
    }
    default:
        break;
    }

    return QGoodWindow::event(event);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
#else
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#endif
{
#ifdef Q_OS_WIN
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MSG *msg = static_cast<MSG*>(message);

    switch (msg->message)
    {
    case WM_THEMECHANGED:
    case WM_DWMCOMPOSITIONCHANGED:
    {
        //Keep window theme on Windows theme change events.
        QTimer::singleShot(1000, this, [=]{
            if (m_dark)
                darkTheme();
            else
                lightTheme();
        });

        break;
    }
    default:
        break;
    }
#endif
#endif
    return QGoodWindow::nativeEvent(eventType, message, result);
}
#endif

void MainWindow::closeEvent(QCloseEvent *event)
{
    int result = QMessageBox::question(this, "Close window", "Are you sure to close?");

    if (result != QMessageBox::Yes)
        event->ignore();
}
