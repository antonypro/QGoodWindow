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

#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(qreal pixel_ratio, QWidget *parent = nullptr);

public slots:
    void setText(const QString &text);
    void setActive(bool active);
    void setTitleColor(const QColor &active_color, const QColor &inactive_color);

private:
    //Functions
    void paintEvent(QPaintEvent *event);

    //Variables
    QString m_title;
    bool m_active;
    QColor m_active_color;
    QColor m_inactive_color;
    qreal m_pixel_ratio;
};

#endif // TITLEWIDGET_H
