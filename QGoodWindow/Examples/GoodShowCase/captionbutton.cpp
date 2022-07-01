/*
The MIT License (MIT)

Copyright © 2018-2022 Antonio Dias

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

#include "captionbutton.h"

CaptionButton::CaptionButton(QWidget *parent) : QWidget(parent)
{
    m_is_active = false;
    m_is_under_mouse = false;
    m_is_pressed = false;
    m_icon_dark = false;

    setAttribute(Qt::WA_Hover);
}

CaptionButton::~CaptionButton()
{

}

QPixmap CaptionButton::drawIcon(const QPixmap &icon, bool active, bool force_light)
{
    QImage tmp = icon.toImage();

    if (!active)
    {
        for (int i = 0; i < tmp.height(); i++)
        {
            for (int j = 0; j < tmp.width(); j++)
            {
                QColor pixel = QColor::fromRgba(tmp.pixel(j, i));

                pixel.setRedF(pixel.redF() * 0.5f);
                pixel.setGreenF(pixel.greenF() * 0.5f);
                pixel.setBlueF(pixel.blueF() * 0.5f);

                tmp.setPixel(j, i, pixel.rgba());
            }
        }
    }

    if (m_icon_dark && !force_light)
        tmp.invertPixels();

    return QPixmap::fromImage(tmp);
}

void CaptionButton::init(IconType type)
{
    m_type = type;

    setColors();
    drawIcons();
}

void CaptionButton::drawIcons()
{
    switch (m_type)
    {
    case IconType::Minimize:
    {
        QPixmap icon = QPixmap(":/images/icon_window_minimize.png");

        m_active_icon = drawIcon(icon, true);
        m_inactive_icon = drawIcon(icon, false);

        break;
    }
    case IconType::Restore:
    {
        QPixmap icon = QPixmap(":/images/icon_window_restore.png");

        m_active_icon = drawIcon(icon, true);
        m_inactive_icon = drawIcon(icon, false);

        break;
    }
    case IconType::Maximize:
    {
        QPixmap icon = QPixmap(":/images/icon_window_maximize.png");

        m_active_icon = drawIcon(icon, true);
        m_inactive_icon = drawIcon(icon, false);

        break;
    }
    case IconType::Close:
    {
        QPixmap icon = QPixmap(":/images/icon_window_close.png");

        m_active_icon = drawIcon(icon, true);
        m_inactive_icon = drawIcon(icon, false);
        m_close_icon_hover = drawIcon(icon, true, true);

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

    repaint();
}

void CaptionButton::setIconMode(bool icon_dark)
{
    m_icon_dark = icon_dark;

    drawIcons();
    setColors();

    repaint();
}

void CaptionButton::setActive(bool is_active)
{
    m_is_active = is_active;

    repaint();
}

void CaptionButton::setState(int state)
{
    switch (state)
    {
    case QEvent::HoverEnter:
    {
        m_is_under_mouse = true;

        repaint();

        break;
    }
    case QEvent::HoverLeave:
    {
        m_is_under_mouse = false;

        repaint();

        break;
    }
    case QEvent::MouseButtonPress:
    {
        m_is_pressed = true;

        m_is_under_mouse = true;

        repaint();

        break;
    }
    case QEvent::MouseButtonRelease:
    {
        m_is_pressed = false;

        m_is_under_mouse = false;

        repaint();

        break;
    }
    default:
        break;
    }
}

void CaptionButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPixmap current_icon = m_active_icon;
    QColor current_color = m_normal;

    //Change icon if needed
    if (m_is_under_mouse)
    {
        if (m_type == IconType::Close)
            current_icon = m_close_icon_hover;
    }
    else
    {
        if (!m_is_active)
            current_icon = m_inactive_icon;
    }

    //Change background color if needed
    if (m_is_pressed)
    {
        if (m_is_under_mouse)
            current_color = m_pressed;
    }
    else
    {
        if (m_is_under_mouse)
            current_color = m_hover;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    painter.fillRect(rect(), current_color);

    QRect target_rect;
    target_rect = current_icon.rect();
    target_rect.setSize(QSize(16, 16));
    target_rect = QRect(rect().center() - target_rect.center(), target_rect.size());
    painter.drawPixmap(target_rect, current_icon);
}
