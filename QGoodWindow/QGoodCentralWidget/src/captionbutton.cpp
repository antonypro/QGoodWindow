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

CaptionButton::CaptionButton(IconType type, qreal pixel_ratio, QWidget *parent) : QWidget(parent)
{
    setVisible(false);

    m_type = type;
    m_pixel_ratio = pixel_ratio;
    m_is_active = false;
    m_is_under_mouse = false;
    m_is_pressed = false;
    m_icon_dark = false;

    setColors();
    drawIcons();
}

CaptionButton::~CaptionButton()
{

}

void CaptionButton::drawIcons()
{
    int w = qCeil(10 * m_pixel_ratio);

    if (w <= 10)
        w = 10;
    else if (w <= 12)
        w = 12;
    else
        w = 15;

    switch (m_type)
    {
    case IconType::Minimize:
    {
        if (m_icon_dark)
        {
            m_active_icon = QIcon(":/icons/minimize-active-dark.ico").pixmap(w, 1);
            m_inactive_icon = QIcon(":/icons/minimize-inactive-dark.ico").pixmap(w, 1);
        }
        else
        {
            m_active_icon = QIcon(":/icons/minimize-active-light.ico").pixmap(w, 1);
            m_inactive_icon = QIcon(":/icons/minimize-inactive-light.ico").pixmap(w, 1);
        }

        break;
    }
    case IconType::Restore:
    {
        if (m_icon_dark)
        {
            m_active_icon = QIcon(":/icons/restore-active-dark.ico").pixmap(w, w);
            m_inactive_icon = QIcon(":/icons/restore-inactive-dark.ico").pixmap(w, w);
        }
        else
        {
            m_active_icon = QIcon(":/icons/restore-active-light.ico").pixmap(w, w);
            m_inactive_icon = QIcon(":/icons/restore-inactive-light.ico").pixmap(w, w);
        }

        break;
    }
    case IconType::Maximize:
    {
        if (m_icon_dark)
        {
            m_active_icon = QIcon(":/icons/maximize-active-dark.ico").pixmap(w, w);
            m_inactive_icon = QIcon(":/icons/maximize-inactive-dark.ico").pixmap(w, w);
        }
        else
        {
            m_active_icon = QIcon(":/icons/maximize-active-light.ico").pixmap(w, w);
            m_inactive_icon = QIcon(":/icons/maximize-inactive-light.ico").pixmap(w, w);
        }

        break;
    }
    case IconType::Close:
    {
        if (m_icon_dark)
        {
            m_active_icon = QIcon(":/icons/close-active-dark.ico").pixmap(w, w);
            m_inactive_icon = QIcon(":/icons/close-inactive-dark.ico").pixmap(w, w);
            m_close_icon_hover = QIcon(":/icons/close-active-light.ico").pixmap(w, w);
        }
        else
        {
            m_active_icon = QIcon(":/icons/close-active-light.ico").pixmap(w, w);
            m_inactive_icon = QIcon(":/icons/close-inactive-light.ico").pixmap(w, w);
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

    update();
}

void CaptionButton::setIconMode(bool icon_dark)
{
    m_icon_dark = icon_dark;

    drawIcons();
    setColors();

    update();
}

void CaptionButton::setActive(bool is_active)
{
    m_is_active = is_active;

    update();
}

void CaptionButton::setState(int state)
{
    switch (state)
    {
    case QEvent::HoverEnter:
    {
        m_is_under_mouse = true;

        update();

        break;
    }
    case QEvent::HoverLeave:
    {
        m_is_under_mouse = false;

        update();

        break;
    }
    case QEvent::MouseButtonPress:
    {
        m_is_pressed = true;

        m_is_under_mouse = true;

        update();

        break;
    }
    case QEvent::MouseButtonRelease:
    {
        m_is_pressed = false;

        m_is_under_mouse = false;

        update();

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
        if (m_type == IconType::Close && m_icon_dark)
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
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    painter.fillRect(rect(), current_color);

    int w = qCeil(10 * m_pixel_ratio);

    if (w <= 10)
        w = 10;
    else if (w <= 12)
        w = 12;
    else
        w = 15;

    int h = (m_type != IconType::Minimize) ? w : 1;

    QRect target_rect;
    target_rect = current_icon.rect();
    target_rect.setSize(QSize(w, h));
    target_rect = QRect(rect().center() - target_rect.center(), target_rect.size());
    painter.drawPixmap(target_rect, current_icon);
}
