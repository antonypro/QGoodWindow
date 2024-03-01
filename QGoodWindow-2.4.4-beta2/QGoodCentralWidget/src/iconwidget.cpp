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

#include "iconwidget.h"

#define ICONWIDTH 16
#define ICONHEIGHT 16

IconWidget::IconWidget(QWidget *parent) : QWidget(parent)
{
    m_active = true;
}

void IconWidget::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;

    QImage tmp = m_pixmap.toImage();

    for (int y = 0; y < tmp.height(); y++)
    {
        QRgb *pixel_ptr = reinterpret_cast<QRgb*>(tmp.scanLine(y));

        for (int x = 0; x < tmp.width(); x++)
        {
            QRgb pixel = pixel_ptr[x];

            int gray = qGray(pixel);

            int alpha = qAlpha(pixel);

            QRgb rgba = qRgba(gray, gray, gray, alpha);

            pixel_ptr[x] = rgba;
        }
    }

    m_grayed_pixmap = QPixmap::fromImage(tmp);

    update();
}

void IconWidget::setActive(bool active)
{
    m_active = active;
    update();
}

void IconWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!isVisible())
        return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter.drawPixmap((width() - ICONWIDTH) / 2, (height() - ICONHEIGHT) / 2,
                       ICONWIDTH, ICONHEIGHT,
                       (m_active ? m_pixmap : m_grayed_pixmap));
}
