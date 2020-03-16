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

#include "iconwidget.h"

#define ICONWIDTH 16
#define ICONHEIGHT 16

IconWidget::IconWidget(qreal pixel_ratio, QWidget *parent) : QWidget(parent)
{
    m_active = true;
    m_pixel_ratio = pixel_ratio;
}

void IconWidget::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;

    QImage tmp = m_pixmap.toImage();

    for (int i = 0; i < m_pixmap.width(); i++)
    {
        for (int j = 0; j < m_pixmap.height(); j++)
        {
            int gray = qGray(tmp.pixel(i, j));

            int alpha = qAlpha(tmp.pixel(i, j));

            tmp.setPixel(i, j, qRgba(gray, gray, gray, alpha));
        }
    }

    m_grayed_pixmap = QPixmap::fromImage(tmp);

    repaint();
}

void IconWidget::setActive(bool active)
{
    m_active = active;
    repaint();
}

void IconWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter.drawPixmap(qRound(width() - ICONWIDTH * m_pixel_ratio) / 2,
                       qRound(height() - ICONHEIGHT * m_pixel_ratio) / 2,
                       qRound(ICONWIDTH * m_pixel_ratio),
                       qRound(ICONHEIGHT * m_pixel_ratio),
                       m_active ? m_pixmap : m_grayed_pixmap);
}
