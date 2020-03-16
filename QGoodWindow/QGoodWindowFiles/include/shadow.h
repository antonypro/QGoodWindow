/*
The MIT License (MIT)

Copyright © 2018-2020 Antonio Dias

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

#define SHADOWWIDTH 10
#define COLOR1 QColor(0, 0, 0, 75)
#define COLOR2 QColor(0, 0, 0, 30)
#define COLOR3 QColor(0, 0, 0, 1)

//\cond HIDDEN_SYMBOLS
class Shadow : public QWidget
{
    Q_OBJECT
public:
    explicit Shadow(HWND hwnd, QWidget *parent = nullptr);

public slots:
    void showLater();
    void show();
    void hide();
    void setActive(bool active);

private:
    //Functions
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void paintEvent(QPaintEvent *event);

    //Variables
    HWND m_hwnd;
    QTimer *m_timer;
    bool m_active;
};
//\endcond

#endif // SHADOW_H
