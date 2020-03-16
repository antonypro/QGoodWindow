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

#include "include/shadow.h"

Shadow::Shadow(HWND hwnd, QWidget *parent) : QWidget(parent)
{
    m_hwnd = hwnd;
    m_active = true;

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool);

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    setAttribute(Qt::WA_TransparentForMouseEvents);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Shadow::show);
    m_timer->setInterval(500);
    m_timer->setSingleShot(true);
}

void Shadow::showLater()
{
    m_timer->stop();
    m_timer->start();
}

void Shadow::show()
{
    if (m_timer->isActive())
        return;

    QWidget::show();
    QWidget::raise();

    SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

void Shadow::hide()
{
    if (!isVisible())
        return;

    QWidget::hide();
}

void Shadow::setActive(bool active)
{
    m_active = active;
    repaint();
}

bool Shadow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);

    switch (msg->message)
    {
    case WM_ACTIVATE:
    {
        switch (msg->wParam)
        {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
        {
            SetForegroundWindow(m_hwnd);
            break;
        }
        default:
            break;
        }

        break;
    }
    case WM_MOUSEACTIVATE:
    {
        SetForegroundWindow(m_hwnd);
        *result = MA_NOACTIVATE;
        return true;
    }
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCHITTEST:
    {
        *result = long(SendMessage(m_hwnd, msg->message, msg->wParam, msg->lParam));
        return true;
    }
    default:
        break;
    }

    return QWidget::nativeEvent(eventType, message, result);
}

void Shadow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPixmap radialgradient = QPixmap(SHADOWWIDTH * 2, SHADOWWIDTH * 2);

    //Draw a radial gradient then split it in 4 parts and draw it to corners
    {
        radialgradient.fill(QColor(0, 0, 0, 1));

        QPainter painter(&radialgradient);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_Source);

        QRadialGradient gradient(SHADOWWIDTH, SHADOWWIDTH, SHADOWWIDTH);
        gradient.setColorAt(0.0, COLOR1);
        gradient.setColorAt(0.2, COLOR2);
        gradient.setColorAt(0.5, COLOR3);

        QPen pen(Qt::transparent, 0);
        painter.setPen(pen);
        painter.setBrush(gradient);
        painter.drawEllipse(0, 0, SHADOWWIDTH * 2, SHADOWWIDTH * 2);
    }

    //Draw shadow
    QPainter painter(this);

    painter.setCompositionMode(QPainter::CompositionMode_Source);

    if (!m_active)
    {
        painter.fillRect(rect(), QColor(0, 0, 0, 1));

        QRect rect1 = rect().adjusted(SHADOWWIDTH, SHADOWWIDTH, -SHADOWWIDTH, -SHADOWWIDTH);

        painter.fillRect(rect1, Qt::transparent);

        return;
    }

    QRect rect1 = rect().adjusted(SHADOWWIDTH, SHADOWWIDTH, -SHADOWWIDTH, -SHADOWWIDTH);

    painter.drawPixmap(0, 0, SHADOWWIDTH, SHADOWWIDTH, radialgradient, 0, 0, SHADOWWIDTH, SHADOWWIDTH); //Top-left corner
    painter.drawPixmap(rect().width() - SHADOWWIDTH, 0, radialgradient, SHADOWWIDTH, 0, SHADOWWIDTH, SHADOWWIDTH); //Top-right corner
    painter.drawPixmap(0, rect().height() - SHADOWWIDTH, radialgradient, 0, SHADOWWIDTH, SHADOWWIDTH, SHADOWWIDTH); //Bottom-left corner
    painter.drawPixmap(rect().width() - SHADOWWIDTH, rect().height() - SHADOWWIDTH, radialgradient, SHADOWWIDTH, SHADOWWIDTH, SHADOWWIDTH, SHADOWWIDTH); //Bottom-right corner

    painter.drawPixmap(SHADOWWIDTH, 0, rect1.width(), SHADOWWIDTH, radialgradient, SHADOWWIDTH, 0, 1, SHADOWWIDTH); //Top
    painter.drawPixmap(0, SHADOWWIDTH, SHADOWWIDTH, rect1.height(), radialgradient, 0, SHADOWWIDTH, SHADOWWIDTH, 1); //Left
    painter.drawPixmap(rect1.width() + SHADOWWIDTH, SHADOWWIDTH, SHADOWWIDTH, rect1.height(), radialgradient, SHADOWWIDTH, SHADOWWIDTH, SHADOWWIDTH, 1); //Right
    painter.drawPixmap(SHADOWWIDTH, rect1.height() + SHADOWWIDTH, rect1.width(), SHADOWWIDTH, radialgradient, SHADOWWIDTH, SHADOWWIDTH, 1, SHADOWWIDTH); //Bottom
}
