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

#include "shadow.h"

#ifdef Q_OS_LINUX
#include "qgoodwindow.h"
#endif

#ifdef Q_OS_WIN
#define SHADOWWIDTH qCeil(10 * m_pixel_ratio)
#define COLOR1 QColor(0, 0, 0, 75)
#define COLOR2 QColor(0, 0, 0, 30)
#define COLOR3 QColor(0, 0, 0, 1)
#endif

#ifdef Q_OS_WIN
Shadow::Shadow(qreal pixel_ratio, HWND hwnd) : QWidget()
{
    m_pixel_ratio = pixel_ratio;

    m_hwnd = hwnd;
    m_active = true;

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool);

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    setAttribute(Qt::WA_TransparentForMouseEvents);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Shadow::show);
    m_timer->setInterval(500/*Time to wait before showing shadow when showLater() is callled.*/);
    m_timer->setSingleShot(true);
}
#endif
#ifdef Q_OS_LINUX
Shadow::Shadow(QWidget *parent) : QWidget(parent)
{
    m_parent = qobject_cast<QGoodWindow*>(parent);

    setWindowFlags(Qt::Window |
                   Qt::FramelessWindowHint |
                   Qt::WindowDoesNotAcceptFocus |
                   Qt::BypassWindowManagerHint);

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}
#endif

int Shadow::shadowWidth()
{
#ifdef Q_OS_WIN
    return SHADOWWIDTH;
#else
    return 0;
#endif
}

void Shadow::showLater()
{
#ifdef Q_OS_WIN
    m_timer->stop();
    m_timer->start();
#endif
}

void Shadow::show()
{
#ifdef Q_OS_WIN
    if (m_timer->isActive())
        return;

    if (!IsWindowEnabled(m_hwnd))
        return;

    QWidget::show();
    QWidget::raise();

    SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
#else
    QWidget::show();
#endif
}

void Shadow::hide()
{
#ifdef Q_OS_WIN
    m_timer->stop();

    if (!isVisible())
        return;

    QWidget::hide();
#else
    QWidget::hide();
#endif
}

void Shadow::setActive(bool active)
{
#ifdef Q_OS_WIN
    m_active = active;
    repaint();
#else
    Q_UNUSED(active)
#endif
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
bool Shadow::nativeEvent(const QByteArray &eventType, void *message, long *result)
#else
bool Shadow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#endif
{
#ifdef Q_OS_WIN
    MSG* msg = static_cast<MSG*>(message);

    switch (msg->message)
    {
    case WM_ACTIVATE:
    {
        switch (msg->wParam)
        {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
        {
            //When shadow got focus, transfer it to main window.
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
        //When shadow got focus, transfer it to main window.
        SetForegroundWindow(m_hwnd);

        //Prevent main window from "focus flickering".
        *result = MA_NOACTIVATE;

        return true;
    }
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCHITTEST:
    {
        //Transfer the above messages to main window,
        //this way the resize and snap effects happens also
        //when interacting with the shadow, and acts like a secondary border.
        *result = long(SendMessageW(m_hwnd, msg->message, msg->wParam, msg->lParam));
        return true;
    }
    default:
        break;
    }
#endif

    return QWidget::nativeEvent(eventType, message, result);
}

void Shadow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

#ifdef Q_OS_WIN
    //Draw shadow

    const int shadow_width = SHADOWWIDTH;

    QPainter painter(this);

    painter.setCompositionMode(QPainter::CompositionMode_Source);

    if (!m_active)
    {
        painter.fillRect(rect(), QColor(0, 0, 0, 1));

        QRect rect1 = rect().adjusted(shadow_width, shadow_width, -shadow_width, -shadow_width);

        painter.fillRect(rect1, Qt::transparent);

        return;
    }

    QPixmap radial_gradient = QPixmap(shadow_width * 2, shadow_width * 2);

    {
        //Draw a radial gradient then split it in 4 parts and draw it to corners and edges

        radial_gradient.fill(QColor(0, 0, 0, 1));

        QPainter painter(&radial_gradient);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_Source);

        QRadialGradient gradient(shadow_width, shadow_width, shadow_width);
        gradient.setColorAt(0.0, COLOR1);
        gradient.setColorAt(0.2, COLOR2);
        gradient.setColorAt(0.5, COLOR3);

        QPen pen(Qt::transparent, 0);
        painter.setPen(pen);
        painter.setBrush(gradient);
        painter.drawEllipse(0, 0, shadow_width * 2, shadow_width * 2);
    }

    QRect rect1 = rect().adjusted(shadow_width, shadow_width, -shadow_width, -shadow_width);

    painter.drawPixmap(0, 0, shadow_width, shadow_width, radial_gradient, 0, 0, shadow_width, shadow_width); //Top-left corner
    painter.drawPixmap(rect().width() - shadow_width, 0, radial_gradient, shadow_width, 0, shadow_width, shadow_width); //Top-right corner
    painter.drawPixmap(0, rect().height() - shadow_width, radial_gradient, 0, shadow_width, shadow_width, shadow_width); //Bottom-left corner
    painter.drawPixmap(rect().width() - shadow_width, rect().height() - shadow_width, radial_gradient, shadow_width, shadow_width, shadow_width, shadow_width); //Bottom-right corner

    painter.drawPixmap(shadow_width, 0, rect1.width(), shadow_width, radial_gradient, shadow_width, 0, 1, shadow_width); //Top
    painter.drawPixmap(0, shadow_width, shadow_width, rect1.height(), radial_gradient, 0, shadow_width, shadow_width, 1); //Left
    painter.drawPixmap(rect1.width() + shadow_width, shadow_width, shadow_width, rect1.height(), radial_gradient, shadow_width, shadow_width, shadow_width, 1); //Right
    painter.drawPixmap(shadow_width, rect1.height() + shadow_width, rect1.width(), shadow_width, radial_gradient, shadow_width, shadow_width, 1, SHADOWWIDTH); //Bottom
#endif
#ifdef Q_OS_LINUX
    if (!m_parent)
        return;

    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect(), Qt::transparent);
#endif
}
