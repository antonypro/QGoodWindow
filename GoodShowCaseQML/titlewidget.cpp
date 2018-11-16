#include "titlewidget.h"

TitleWidget::TitleWidget(QWidget *parent) : QWidget(parent)
{
    m_active = false;
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
    font.setPointSize(9);
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
