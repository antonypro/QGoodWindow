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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent)
{
#ifdef QGOODWINDOW
    title_bar = new TitleBar(pixelRatio(), this);

    frame = new QFrame(this);

    m_frame_style = QString("QFrame {background-color: %0; border: %1;}");

    m_color_str = "#FFFFFF";

#ifndef Q_OS_MAC
    frame->setStyleSheet(m_frame_style.arg(m_color_str).arg("1px solid #1883D7"));
#else
    frame->setStyleSheet(m_frame_style.arg(m_color_str).arg("none"));
#endif

    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(title_bar);
    layout->addStretch(1);

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
            title_bar->setIcon(icon.pixmap(16, 16));
    });
#else
    QString m_frame_style = QString("QFrame {background-color: #FFFFFF;}");

    QFrame *frame = new QFrame(this);
    frame->setStyleSheet(m_frame_style);

    setCentralWidget(frame);
#endif
    resize(qRound(640 * pixelRatio()), qRound(480 * pixelRatio()));

    move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());

#ifdef QGOODWINDOW
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_S), this);

    connect(shortcut, &QShortcut::activated, this, [=]{
        title_bar->setDarkMode(!title_bar->dark());

        if (!title_bar->dark())
            m_color_str = "#FFFFFF";
        else
            m_color_str = "#000000";

#ifndef Q_OS_MAC
        frame->setStyleSheet(m_frame_style.arg(m_color_str)
                             .arg((isFullScreen() || isMaximized()) ? "none" : "1px solid #1883D7"));
#else
        frame->setStyleSheet(m_frame_style.arg(m_color_str).arg("none"));
#endif
    });
#endif
    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::Key_F), this);

    connect(shortcut1, &QShortcut::activated, this, [=]{
        if (!isFullScreen())
            showFullScreen();
        else
            showNormal();
    });

    QPixmap p = QPixmap(1, 1);
    p.fill(Qt::red);

    setWindowIcon(p);

#ifdef QGOODWINDOW
    setWindowTitle("Good Window - Press S to toggle theme - Press F to toggle fullscreen!");
#else
    setWindowTitle("Good Window - Press F to toggle fullscreen!");
#endif
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int result = QMessageBox::question(this, "Close window", "Are you sure to close?");

    if (result != QMessageBox::Yes)
        event->ignore();
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type())
    {
#ifdef QGOODWINDOW
    case QEvent::Show:
    case QEvent::WindowStateChange:
    {
#ifndef Q_OS_MAC
        frame->setStyleSheet(m_frame_style.arg(m_color_str)
                             .arg((isFullScreen() || isMaximized()) ? "none" : "1px solid #1883D7"));
#endif
        title_bar->setMaximized(isFullScreen() || isMaximized());

        title_bar->setVisible(!isFullScreen());

        if (!isFullScreen())
            setMargins(qRound(30 * pixelRatio()), qRound(30 * pixelRatio()), 0, qRound(36 * 3 * pixelRatio()));
        else
            setMargins(0, 0, 0, 0);

        break;
    }
    case QEvent::WindowActivate:
    {
#ifndef Q_OS_MAC
        frame->setStyleSheet(m_frame_style.arg(m_color_str)
                             .arg((isFullScreen() || isMaximized()) ? "none" : "1px solid #1883D7"));
#endif
        title_bar->setActive(true);

        break;
    }
    case QEvent::WindowDeactivate:
    {
#ifndef Q_OS_MAC
        frame->setStyleSheet(m_frame_style.arg(m_color_str)
                             .arg((isFullScreen() || isMaximized()) ? "none" : "1px solid #AAAAAA"));
#endif
        title_bar->setActive(false);

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
