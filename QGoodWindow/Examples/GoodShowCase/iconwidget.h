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

#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class IconWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IconWidget(qreal pixel_ratio, QWidget *parent = nullptr);

public slots:
    void setPixmap(const QPixmap &pixmap);
    void setActive(bool active);

private:
    //Functions
    void paintEvent(QPaintEvent *event);

    //Variables
    QPixmap m_pixmap;
    QPixmap m_grayed_pixmap;
    bool m_active;
    qreal m_pixel_ratio;
};

#endif // ICONWIDGET_H
