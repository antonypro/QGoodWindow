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

    painter.drawPixmap((width() - ICONWIDTH) / 2, (height() - ICONHEIGHT) / 2, ICONWIDTH, ICONHEIGHT,
                       m_active ? m_pixmap : m_grayed_pixmap);
}
