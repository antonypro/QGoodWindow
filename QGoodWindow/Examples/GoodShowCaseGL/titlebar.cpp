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

#include "titlebar.h"

TitleBar::TitleBar(qreal pixel_ratio, QWidget *parent) : QFrame(parent)
{
    style = QString("TitleBar {background-color: %0; border: none;}");

    setFixedHeight(qRound(29 * pixel_ratio));

    iconwidget = new IconWidget(pixel_ratio, this);
    iconwidget->setFixedWidth(qRound(29 * pixel_ratio));

    titlewidget = new TitleWidget(pixel_ratio, this);

    minbtn = new CaptionButton(CaptionButton::IconType::Minimize, pixel_ratio, this);
    minbtn->setFixedSize(qRound(36 * pixel_ratio), qRound(29 * pixel_ratio));

    restorebtn = new CaptionButton(CaptionButton::IconType::Restore, pixel_ratio, this);
    restorebtn->setFixedSize(qRound(36 * pixel_ratio), qRound(29 * pixel_ratio));

    maxbtn = new CaptionButton(CaptionButton::IconType::Maximize, pixel_ratio, this);
    maxbtn->setFixedSize(qRound(36 * pixel_ratio), qRound(29 * pixel_ratio));

    clsbtn = new CaptionButton(CaptionButton::IconType::Close, pixel_ratio, this);
    clsbtn->setFixedSize(qRound(36 * pixel_ratio), qRound(29 * pixel_ratio));

    connect(minbtn, &CaptionButton::clicked, this, &TitleBar::showMinimized);
    connect(restorebtn, &CaptionButton::clicked, this, &TitleBar::showNormal);
    connect(maxbtn, &CaptionButton::clicked, this, &TitleBar::showMaximized);
    connect(clsbtn, &CaptionButton::clicked, this, &TitleBar::closeWindow);

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);

    hlayout->addWidget(iconwidget);
    hlayout->addWidget(titlewidget);
    hlayout->addWidget(minbtn);
    hlayout->addWidget(restorebtn);
    hlayout->addWidget(maxbtn);
    hlayout->addWidget(clsbtn);

    m_active = true;

    m_is_maximized = false;

    setDarkMode(false);
}

void TitleBar::setTitle(const QString &title)
{
    titlewidget->setText(title);
}

void TitleBar::setIcon(const QPixmap &icon)
{
    iconwidget->setPixmap(icon);
}

void TitleBar::setActive(bool active)
{
    m_active = active;

    iconwidget->setActive(active);
    titlewidget->setActive(active);
    minbtn->setActive(active);
    restorebtn->setActive(active);
    maxbtn->setActive(active);
    clsbtn->setActive(active);
}

void TitleBar::setMaximized(bool maximized)
{
    m_is_maximized = maximized;

    if (maximized)
    {
        restorebtn->setVisible(true);
        maxbtn->setVisible(false);
    }
    else
    {
        restorebtn->setVisible(false);
        maxbtn->setVisible(true);
    }
}

bool TitleBar::dark()
{
    return m_dark;
}

void TitleBar::setDarkMode(bool dark)
{
    m_dark = dark;

    if (dark)
    {
        setStyleSheet(style.arg("#000000"));
        update();

        //Light mode to contrast
        minbtn->setIconMode(false);
        maxbtn->setIconMode(false);
        restorebtn->setIconMode(false);
        clsbtn->setIconMode(false);

        titlewidget->setTitleColor(QColor(255, 255, 255), QColor(150, 150, 150));

        setActive(m_active);
    }
    else
    {
        setStyleSheet(style.arg("#FFFFFF"));
        update();

        //Dark mode to contrast
        minbtn->setIconMode(true);
        maxbtn->setIconMode(true);
        restorebtn->setIconMode(true);
        clsbtn->setIconMode(true);

        titlewidget->setTitleColor(QColor(0, 0, 0), QColor(150, 150, 150));

        setActive(m_active);
    }
}

void TitleBar::captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state)
{
    switch (state)
    {
        // Hover enter
    case QGoodWindow::CaptionButtonState::MinimizeHoverEnter:
    {
        minbtn->setState(QEvent::HoverEnter);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeHoverEnter:
    {
        if (!m_is_maximized)
            maxbtn->setState(QEvent::HoverEnter);
        else
            restorebtn->setState(QEvent::HoverEnter);

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseHoverEnter:
    {
        clsbtn->setState(QEvent::HoverEnter);

        break;
    }
        // Hover leave
    case QGoodWindow::CaptionButtonState::MinimizeHoverLeave:
    {
        minbtn->setState(QEvent::HoverLeave);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeHoverLeave:
    {
        if (!m_is_maximized)
            maxbtn->setState(QEvent::HoverLeave);
        else
            restorebtn->setState(QEvent::HoverLeave);

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseHoverLeave:
    {
        clsbtn->setState(QEvent::HoverLeave);

        break;
    }
        // Mouse button press
    case QGoodWindow::CaptionButtonState::MinimizePress:
    {
        minbtn->setState(QEvent::MouseButtonPress);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizePress:
    {
        if (!m_is_maximized)
            maxbtn->setState(QEvent::MouseButtonPress);
        else
            restorebtn->setState(QEvent::MouseButtonPress);

        break;
    }
    case QGoodWindow::CaptionButtonState::ClosePress:
    {
        clsbtn->setState(QEvent::MouseButtonPress);

        break;
    }
        // Mouse button release
    case QGoodWindow::CaptionButtonState::MinimizeRelease:
    {
        minbtn->setState(QEvent::MouseButtonRelease);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeRelease:
    {
        if (!m_is_maximized)
            maxbtn->setState(QEvent::MouseButtonRelease);
        else
            restorebtn->setState(QEvent::MouseButtonRelease);

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseRelease:
    {
        clsbtn->setState(QEvent::MouseButtonRelease);

        break;
    }
        // Mouse button clicked
    case QGoodWindow::CaptionButtonState::MinimizeClicked:
    {
        emit minbtn->clicked();

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeClicked:
    {
        if (!m_is_maximized)
            emit maxbtn->clicked();
        else
            emit restorebtn->clicked();

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseClicked:
    {
        emit clsbtn->clicked();

        break;
    }
    default:
        break;
    }
}
