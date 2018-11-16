#include "captionbutton.h"

CaptionButton::CaptionButton(IconType type, qreal pixelRatio, QWidget *parent) : QWidget(parent)
{
    m_type = type;
    m_pixel_ratio = pixelRatio;
    m_is_pressed = false;
    m_icon_dark = false;

    setAttribute(Qt::WA_Hover);

    setColors();
    drawIcons();
}

void CaptionButton::drawIcons()
{
    int w = qRound(10 * m_pixel_ratio);

    m_active_icon = QPixmap(w, w);
    m_inactive_icon = QPixmap(w, w);

    m_active_icon.fill(Qt::transparent);
    m_inactive_icon.fill(Qt::transparent);

    QPainter painter1(&m_active_icon);
    QPainter painter2(&m_inactive_icon);

    QPen pen1;
    pen1.setWidthF(1);

    if (!m_icon_dark)
        pen1.setColor(QColor(255, 255, 255, 255));
    else
        pen1.setColor(QColor(0, 0, 0, 255));

    QPen pen2;
    pen2.setWidthF(1);

    if (!m_icon_dark)
        pen2.setColor(QColor(255, 255, 255, 150));
    else
        pen2.setColor(QColor(100, 100, 100, 150));

    painter1.setPen(pen1);
    painter2.setPen(pen2);

    switch (m_type)
    {
    case IconType::Minimize:
    {
        int w1 = m_active_icon.width();
        int w2 = m_inactive_icon.width();

        int h1 = m_active_icon.height() / 2;
        int h2 = m_inactive_icon.height() / 2;

        painter1.drawLine(0, h1 - 1, w1 - 1, h1 - 1);

        painter2.drawLine(0, h2 - 1, w2 - 1, h2 - 1);

        break;
    }
    case IconType::Restore:
    {
        int w1 = m_active_icon.width();
        int w2 = m_inactive_icon.width();

        painter1.drawRect(QRectF(2, 0, 0.8 * w1 - 1, 0.8 * w1 - 1));
        painter1.drawRect(QRectF(0, 2, 0.8 * w1 - 1, 0.8 * w1 - 1));

        painter1.setCompositionMode(QPainter::CompositionMode_Clear);

        painter1.fillRect(QRectF(1, 2 + 1, 0.8 * w1 - 2, 0.8 * w1 - 2), Qt::transparent);

        painter2.drawRect(QRectF(2, 0, 0.8 * w2 - 1, 0.8 * w2 - 1));
        painter2.drawRect(QRectF(0, 2, 0.8 * w2 - 1, 0.8 * w2 - 1));

        painter2.setCompositionMode(QPainter::CompositionMode_Clear);

        painter2.fillRect(QRectF(1, 2 + 1, 0.8 * w2 - 2, 0.8 * w2 - 2), Qt::transparent);

        break;
    }
    case IconType::Maximize:
    {
        int w1 = m_active_icon.width();
        int w2 = m_inactive_icon.width();

        painter1.drawRect(QRectF(0, 0, w1 - 1,  w1 - 1));

        painter2.drawRect(QRectF(0, 0, w2 - 1,  w2 - 1));

        break;
    }
    case IconType::Close:
    {
        int w1 = m_active_icon.width();
        int w2 = m_inactive_icon.width();

        int h1 = m_active_icon.height();
        int h2 = m_inactive_icon.height();

        painter1.drawLine(0, 0, w1 - 1, h1 - 1);
        painter1.drawLine(w1 - 1, 0, 0, h1 - 1);

        painter2.drawLine(0, 0, w2 - 1, h2 - 1);
        painter2.drawLine(w2 - 1, 0, 0, h2 - 1);

        if (m_icon_dark)
        {
            m_close_icon_hover = QPixmap(w, w);
            m_close_icon_hover.fill(Qt::transparent);

            QPainter painter(&m_close_icon_hover);

            QPen pen;
            pen.setWidthF(1);
            pen.setColor(QColor(255, 255, 255, 255));

            painter.setPen(pen);

            int w = m_close_icon_hover.width();
            int h = m_close_icon_hover.height();

            painter.drawLine(0, 0, w - 1, h - 1);
            painter.drawLine(w - 1, 0, 0, h - 1);
        }

        break;
    }
    }
}

void CaptionButton::setColors()
{
    if (m_icon_dark)
    {
        if (m_type == IconType::Close)
        {
            m_normal = QColor("transparent");
            m_hover = QColor("#F00000");
            m_pressed = QColor("#F1707A");
        }
        else
        {
            m_normal = QColor("transparent");
            m_hover = QColor("#E5E5E5");
            m_pressed = QColor("#CACACB");
        }
    }
    else
    {
        if (m_type == IconType::Close)
        {
            m_normal = QColor("transparent");
            m_hover = QColor("#F00000");
            m_pressed = QColor("#F1707A");
        }
        else
        {
            m_normal = QColor("transparent");
            m_hover = QColor("#505050");
            m_pressed = QColor("#3F3F3F");
        }
    }

    m_current_color = m_normal;

    repaint();
}

void CaptionButton::setIconMode(bool icon_dark)
{
    m_icon_dark = icon_dark;

    setColors();
    drawIcons();

    repaint();
}

void CaptionButton::setActive(bool active_window)
{
    if (active_window)
    {
        if (m_type == IconType::Close && m_icon_dark && m_is_pressed)
            m_current_icon = m_close_icon_hover;
        else
            m_current_icon = m_active_icon;

        m_last_icon = m_active_icon;
    }
    else
    {
        m_current_icon = m_inactive_icon;
        m_last_icon = m_current_icon;
    }

    repaint();
}

void CaptionButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    painter.fillRect(rect(), m_current_color);

    QRect target_rect;
    target_rect = m_current_icon.rect();
    target_rect.setSize(target_rect.size() * m_pixel_ratio);
    target_rect = QRect(rect().center() - target_rect.center(), target_rect.size());

    painter.drawPixmap(target_rect, m_current_icon);
}

bool CaptionButton::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::HoverEnter:
    {
        m_last_icon = m_current_icon;

        if (m_type == IconType::Close && m_icon_dark)
            m_current_icon = m_close_icon_hover;
        else
            m_current_icon = m_active_icon;

        m_current_color = m_hover;

        repaint();

        break;
    }
    case QEvent::HoverLeave:
    {
        m_current_icon = m_last_icon;

        m_current_color = m_normal;

        repaint();

        break;
    }
    case QEvent::MouseMove:
    {
        if (m_is_pressed && underMouse())
        {
            if (m_type == IconType::Close && m_icon_dark)
                m_current_icon = m_close_icon_hover;

            m_current_color = m_pressed;

            repaint();
        }
        else if (underMouse())
        {
            if (m_type == IconType::Close && m_icon_dark)
                m_current_icon = m_close_icon_hover;

            m_last_icon = m_active_icon;

            m_current_color = m_hover;

            repaint();
        }
        else
        {
            if (m_type == IconType::Close && m_icon_dark)
                m_current_icon = m_active_icon;

            m_last_icon = m_active_icon;

            m_current_color = m_normal;

            repaint();
        }

        break;
    }
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);

        if (e->button() != Qt::LeftButton)
            break;

        m_is_pressed = true;

        if (m_type == IconType::Close && m_icon_dark)
            m_current_icon = m_close_icon_hover;

        m_current_color = m_pressed;

        repaint();

        break;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);

        if (e->button() != Qt::LeftButton)
            break;

        m_is_pressed = false;

        if (m_type == IconType::Close && m_icon_dark)
            m_current_icon = m_active_icon;

        m_current_color = m_normal;

        repaint();

        if (underMouse())
            emit clicked();

        break;
    }
    default:
        break;
    }

    return QWidget::event(event);
}

bool CaptionButton::underMouse()
{
    return (rect().contains(mapFromGlobal(QCursor::pos())));
}
