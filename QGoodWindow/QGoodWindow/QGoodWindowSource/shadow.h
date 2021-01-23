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

#ifndef SHADOW_H
#define SHADOW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#ifdef Q_OS_WIN

#define SHADOWWIDTH qCeil(10 * m_pixel_ratio)
#define COLOR1 QColor(0, 0, 0, 75)
#define COLOR2 QColor(0, 0, 0, 30)
#define COLOR3 QColor(0, 0, 0, 1)

#endif

#ifdef Q_OS_LINUX

#define BORDERWIDTH 8 //PIXELS

#define MOVERESIZE_MOVE 8 //X11 Fixed Value

#define NO_WHERE -1
#define TOP_LEFT 0
#define TOP 1
#define TOP_RIGHT 2
#define LEFT 7
#define RIGHT 3
#define BOTTOM_LEFT 6
#define BOTTOM 5
#define BOTTOM_RIGHT 4
#define TITLE_BAR 8
#endif

#ifndef Q_OS_WIN
class QGoodWindow;
#endif

//\cond HIDDEN_SYMBOLS
class Shadow : public QWidget
{
    Q_OBJECT
public:
#ifdef Q_OS_WIN
    explicit Shadow(qreal pixel_ratio, HWND hwnd);
#else
    explicit Shadow(QWidget *parent);
#endif

public slots:
    void showLater();
    void show();
    void hide();
    void setActive(bool active);

private:
    //Functions
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void paintEvent(QPaintEvent *event);

#ifdef Q_OS_WIN
    //Variables
    HWND m_hwnd;
    QTimer *m_timer;
    bool m_active;
    qreal m_pixel_ratio;
#endif
#ifdef Q_OS_LINUX
    QPointer<QGoodWindow> m_parent;
#endif
};
//\endcond

#endif // SHADOW_H
