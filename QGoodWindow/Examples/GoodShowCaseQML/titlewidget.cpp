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

#include "titlewidget.h"

TitleWidget::TitleWidget(qreal pixel_ratio, QWidget *parent) : QWidget(parent)
{
    m_active = false;
    m_pixel_ratio = pixel_ratio;
}

void TitleWidget::setText(const QString &text)
{
    m_title = text;
    repaint();
}

void TitleWidget::setActive(bool active)
{
    m_active = active;
    repaint();
}

void TitleWidget::setTitleColor(const QColor &active_color, const QColor &inactive_color)
{
    m_active_color = active_color;
    m_inactive_color = inactive_color;

    repaint();
}

void TitleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QFont font;
    font.setPixelSize(qRound(12 * m_pixel_ratio));
    font.setFamily("Segoe UI");

    painter.setFont(font);

    QPen pen;
    pen.setColor(m_active ? m_active_color : m_inactive_color);

    painter.setPen(pen);

    QFontMetrics metrics(painter.font());
    QSize title_size = metrics.size(0, m_title);

    QString title = metrics.elidedText(m_title, Qt::ElideRight, width());

    painter.drawText(0, (height() - title_size.height()) / 2, title_size.width(), title_size.height(), 0, title);
}
