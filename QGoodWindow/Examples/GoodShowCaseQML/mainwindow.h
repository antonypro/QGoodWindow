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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtQml>
#include <QtQuick>
#include <QtQuickWidgets>
#include <QGoodWindow>
#include <QGoodCentralWidget>

#ifdef Q_OS_WIN
#ifdef QT_VERSION_QT5
#include <QtWinExtras>
#endif
#endif

inline qreal pixelRatio()
{
#ifdef QGOODWINDOW
    QScreen *screen = QApplication::primaryScreen();

#ifdef Q_OS_MAC
    qreal pixel_ratio = screen->devicePixelRatio();
#else
    qreal pixel_ratio = screen->logicalDotsPerInch() / qreal(96);
#endif

#else
    qreal pixel_ratio = qreal(1);
#endif
    return pixel_ratio;
}

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //Functions
    void closeEvent(QCloseEvent *event);

    //Variables
    QGoodCentralWidget *m_central_widget;
};

#endif // MAINWINDOW_H
