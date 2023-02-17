/*
The MIT License (MIT)

Copyright © 2018-2023 Antonio Dias (https://github.com/antonypro)

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

CaptionButton::CaptionButton(IconType type, QWidget *parent) : QWidget(parent)
{
    setVisible(false);

    m_type = type;
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

QPixmap CaptionButton::loadSVG(const QString &svg_path, int w, int h,
                               const Qt::TransformationMode &mode)
{
    const qreal scale_factor = qBound(qreal(1), qreal(qgetenv("QGOODWINDOW_SCALE_FACTOR").toDouble()), qreal(2));

    w = qCeil(w * scale_factor);
    h = qCeil(h * scale_factor);

    //Fix bug that makes icon semi-transparent
    QPixmap pix = QIcon(svg_path).pixmap(w * 2, h * 2);

    //Resize to desired size
    pix = pix.scaled(w, h, Qt::KeepAspectRatio, mode);

    return pix;
}

void CaptionButton::paintIcons(const QPixmap &pix_in, bool dark,
                               QPixmap *pix_active_out, QPixmap *pix_inactive_out)
{
    QImage img_active = pix_in.toImage();
    QImage img_inactive = img_active;

    const qreal grayed_reduction = qreal(0.40);

    for (int y = 0; y < img_inactive.height(); y++)
    {
        QRgb *pixel_ptr = reinterpret_cast<QRgb*>(img_inactive.scanLine(y));

        for (int x = 0; x < img_inactive.width(); x++)
        {
            QRgb pixel = pixel_ptr[x];

            QRgb rgba = qRgba(qRound(qRed(pixel) * grayed_reduction),
                              qRound(qGreen(pixel) * grayed_reduction),
                              qRound(qBlue(pixel) * grayed_reduction),
                              qAlpha(pixel));

            pixel_ptr[x] = rgba;
        }
    }

    if (dark)
    {
        img_active.invertPixels();
        img_inactive.invertPixels();
    }

    if (pix_active_out)
        *pix_active_out = QPixmap::fromImage(img_active);

    if (pix_inactive_out)
        *pix_inactive_out = QPixmap::fromImage(img_inactive);
}

void CaptionButton::drawIcons()
{
    const int size = 10;

    const int w = size;
    const int h = size;

    switch (m_type)
    {
    case IconType::Minimize:
    {
        QPixmap icon = loadSVG(":/icons/minimize.svg", w, h, Qt::FastTransformation);

        paintIcons(icon, m_icon_dark, &m_active_icon, &m_inactive_icon);

        break;
    }
    case IconType::Restore:
    {
        QPixmap icon = loadSVG(":/icons/restore.svg", w, h, Qt::SmoothTransformation);

        paintIcons(icon, m_icon_dark, &m_active_icon, &m_inactive_icon);

        break;
    }
    case IconType::Maximize:
    {
        QPixmap icon = loadSVG(":/icons/maximize.svg", w, h, Qt::SmoothTransformation);

        paintIcons(icon, m_icon_dark, &m_active_icon, &m_inactive_icon);

        break;
    }
    case IconType::Close:
    {
        QPixmap icon = loadSVG(":/icons/close.svg", w, h, Qt::SmoothTransformation);

        paintIcons(icon, m_icon_dark, &m_active_icon, &m_inactive_icon);

        if (m_icon_dark)
            paintIcons(icon, false/*dark*/, &m_close_icon_hover, nullptr);

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

    QRect target_rect;
    target_rect = current_icon.rect();
    target_rect.moveCenter(rect().center());
    painter.drawPixmap(target_rect, current_icon);
}
