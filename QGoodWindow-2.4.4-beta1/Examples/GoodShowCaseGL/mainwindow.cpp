/*
The MIT License (MIT)

Copyright © 2018-2024 Antonio Dias (https://github.com/antonypro)

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
    m_good_central_widget = new QGoodCentralWidget(this);

    GLWidget *gl = new GLWidget(this);

    QTimer *gl_timer = new QTimer(this);
    connect(gl_timer, &QTimer::timeout, gl, &GLWidget::rotate);
    gl_timer->setInterval(10);

#ifdef QGOODWINDOW
    m_good_central_widget->setUnifiedTitleBarAndCentralWidget(true);
    m_good_central_widget->setIconVisibility(QGoodCentralWidget::IconVisibilityType::IconOnLeftOfTitle);

    qGoodStateHolder->setCurrentThemeDark(true);

    QGoodWindow::setAppDarkTheme();
    setNativeDarkModeEnabledOnWindows(true);
#endif

    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::Key_F), this);

    connect(shortcut1, &QShortcut::activated, this, [=]{
        if (!isFullScreen())
            showFullScreen();
        else
            showNormal();
    });

    m_good_central_widget->setCentralWidget(gl);
    setCentralWidget(m_good_central_widget);

    QPixmap p = QPixmap(1, 1);
    p.fill(Qt::red);

    setWindowIcon(p);
    setWindowTitle("Good Window - Press F to toggle fullscreen!");

    resize(640, 480);
    move(qApp->primaryScreen()->availableGeometry().center() - rect().center());

#ifdef Q_OS_WIN
#ifdef QT_VERSION_QT5
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

#ifdef QGOODWINDOW
    m_good_central_widget->setTitleAlignment(Qt::AlignCenter);
#endif
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgbox(this);
    msgbox.setIcon(QMessageBox::Question);
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox.setDefaultButton(QMessageBox::No);
    msgbox.setText("Are you sure to close?");

    int result = QGoodCentralWidget::execDialogWithWindow(&msgbox, this);

    if (result != QMessageBox::Yes)
        event->ignore();
}
