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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGoodWindow>

#ifdef Q_OS_WIN
#include <QtWinExtras>
#endif

#ifdef QGOODWINDOW
#include "titlebar.h"
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
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    bool event(QEvent *event);

    //Variables
    bool m_dark;
    QFrame *frame;
    QString m_color_str;
    QString m_frame_style;
#ifdef QGOODWINDOW
    TitleBar *title_bar;
#endif
};

#endif // MAINWINDOW_H
