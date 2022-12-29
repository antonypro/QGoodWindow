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

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent, QColor("#303030"))
{
    m_central_widget = new QGoodCentralWidget(this);

    QQuickWidget *view_widget = new QQuickWidget(this);
    view_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view_widget->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

#ifdef QGOODWINDOW
    connect(this, &QGoodWindow::systemThemeChanged, this, [=]{
        qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());
    });

    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::Key_S), this);

    connect(shortcut1, &QShortcut::activated, this, [=]{
        qGoodStateHolder->setCurrentThemeDark(!qGoodStateHolder->isCurrentThemeDark());
    });
#endif

    QShortcut *shortcut2 = new QShortcut(QKeySequence(Qt::Key_F), this);

    connect(shortcut2, &QShortcut::activated, this, [=]{
        if (!isFullScreen())
            showFullScreen();
        else
            showNormal();
    });

    qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());

    m_central_widget->setCentralWidget(view_widget);
    setCentralWidget(m_central_widget);

    QPixmap p = QPixmap(1, 1);
    p.fill(Qt::red);

    setWindowIcon(p);
    setWindowTitle("Good Window - Press S to toggle theme - Press F to toggle fullscreen!");

    resize(qRound(640 * pixelRatio()), qRound(480 * pixelRatio()));
    move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());

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
