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

#ifdef Q_OS_LINUX
class QGoodWindow;
#endif

//\cond HIDDEN_SYMBOLS
class Shadow : public QWidget
{
    Q_OBJECT
public:
#ifdef Q_OS_WIN
    explicit Shadow(qreal pixel_ratio, HWND hwnd);
#endif
#ifdef Q_OS_LINUX
    explicit Shadow(QWidget *parent);
#endif

public slots:
    void showLater();
    void show();
    void hide();
    void setActive(bool active);
    int shadowWidth();

private:
    //Functions
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void paintEvent(QPaintEvent *event);

    //Variables
#ifdef Q_OS_WIN
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
