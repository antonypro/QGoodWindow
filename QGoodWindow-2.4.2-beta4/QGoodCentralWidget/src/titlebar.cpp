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

#include "titlebar.h"

TitleBar::TitleBar(QGoodWindow *gw, QWidget *parent) : QFrame(parent)
{
    m_layout_spacing = 0;

    // Get Layout Spacing
    {
        QString old_style;

        if (!qApp->style()->objectName().startsWith("fusion"))
        {
            old_style = qApp->style()->objectName();
            qApp->setStyle(QStyleFactory::create("fusion"));
        }

        QWidget widget;

        QHBoxLayout *layout = new QHBoxLayout(&widget);

        m_layout_spacing = layout->spacing();

        if (!old_style.isEmpty())
        {
            qApp->setStyle(QStyleFactory::create(old_style));
        }
    }

    m_gw = gw;

    m_style = QString("TitleBar {background-color: %0;"
                  #ifdef Q_OS_LINUX
                      "border-top-left-radius: 10px; border-top-right-radius: 10px;"
                  #endif
                      "}");

    connect(qGoodStateHolder, &QGoodStateHolder::currentThemeChanged, this, &TitleBar::setTheme);

    setFixedHeight(29);

    m_left_margin_widget_place_holder = new QWidget(this);
    m_left_margin_widget_place_holder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    m_icon_widget = new IconWidget(this);
    m_icon_widget->setFixedWidth(29);

    m_title_widget = new TitleWidget(this, this);

    m_caption_buttons = new QWidget(this);
    m_caption_buttons->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    m_min_btn = new CaptionButton(CaptionButton::IconType::Minimize, m_caption_buttons);

    m_restore_btn = new CaptionButton(CaptionButton::IconType::Restore, m_caption_buttons);

    m_max_btn = new CaptionButton(CaptionButton::IconType::Maximize, m_caption_buttons);

    m_cls_btn = new CaptionButton(CaptionButton::IconType::Close, m_caption_buttons);

    connect(m_min_btn, &CaptionButton::clicked, this, &TitleBar::showMinimized);
    connect(m_restore_btn, &CaptionButton::clicked, this, &TitleBar::showNormal);
    connect(m_max_btn, &CaptionButton::clicked, this, &TitleBar::showMaximized);
    connect(m_cls_btn, &CaptionButton::clicked, this, &TitleBar::closeWindow);

    QHBoxLayout *hlayout = new QHBoxLayout(m_caption_buttons);
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);

    hlayout->addWidget(m_min_btn);
    hlayout->addWidget(m_restore_btn);
    hlayout->addWidget(m_max_btn);
    hlayout->addWidget(m_cls_btn);

    m_caption_buttons->adjustSize();

    QStackedLayout *stacked_layout = new QStackedLayout(this);
    stacked_layout->setStackingMode(QStackedLayout::StackAll);

    QWidget *widget = new QWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    stacked_layout->addWidget(widget);
    stacked_layout->addWidget(m_title_widget);

    m_left_widget_place_holder = new QWidget(this);
    m_left_widget_place_holder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_left_widget_place_holder->setVisible(false);
    m_left_widget_place_holder->setEnabled(false);

    m_right_widget_place_holder = new QWidget(this);
    m_right_widget_place_holder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_right_widget_place_holder->setVisible(false);
    m_right_widget_place_holder->setEnabled(false);

    m_center_widget_place_holder = new QWidget(this);
    m_center_widget_place_holder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_center_widget_place_holder->setVisible(false);
    m_center_widget_place_holder->setEnabled(false);

    m_center_spacer_item_left = new QSpacerItem(0, 0);
    m_center_spacer_item_right = new QSpacerItem(0, 0);

    layout->addWidget(m_left_margin_widget_place_holder);
    layout->addWidget(m_icon_widget);
    layout->addWidget(m_left_widget_place_holder);
    layout->addStretch();
    layout->addSpacerItem(m_center_spacer_item_left);
    layout->addWidget(m_center_widget_place_holder);
    layout->addSpacerItem(m_center_spacer_item_right);
    layout->addStretch();
    layout->addWidget(m_right_widget_place_holder);
    layout->addWidget(m_caption_buttons);

    QTimer::singleShot(0, this, [=]{
        //All caption button starts in hidden state,
        //showing them as needed.

        if (isMinimizedButtonEnabled())
        {
            m_min_btn->setVisible(true);
        }

        if (isMaximizeButtonEnabled())
        {
            if (!isMaximized())
                m_max_btn->setVisible(true);
            else
                m_restore_btn->setVisible(true);
        }

        m_cls_btn->setVisible(true);

        m_caption_buttons->adjustSize();
    });

    m_active = true;

    m_is_maximized = false;

    setTheme();
}

void TitleBar::setTitle(const QString &title)
{
    m_title_widget->setText(title);
}

void TitleBar::setIcon(const QPixmap &icon)
{
    m_icon_widget->setPixmap(icon);
}

void TitleBar::setActive(bool active)
{
    m_active = active;

    m_icon_widget->setActive(active);
    m_title_widget->setActive(active);
    m_min_btn->setActive(active);
    m_restore_btn->setActive(active);
    m_max_btn->setActive(active);
    m_cls_btn->setActive(active);
}

void TitleBar::setTitleAlignment(const Qt::Alignment &alignment)
{
    m_title_widget->setTitleAlignment(alignment);
}

void TitleBar::setMaximized(bool maximized)
{
    m_is_maximized = maximized;

    if (!isMaximizeButtonEnabled())
        return;

    if (maximized)
    {
        m_max_btn->setVisible(false);
        m_restore_btn->setVisible(true);
    }
    else
    {
        m_restore_btn->setVisible(false);
        m_max_btn->setVisible(true);
    }

    m_caption_buttons->adjustSize();
}

void TitleBar::setTheme()
{
    bool dark = qGoodStateHolder->isCurrentThemeDark();

    setAttribute(Qt::WA_TranslucentBackground, false);

    if (dark)
    {
        QTimer::singleShot(0, this, [=]{
            if (m_title_bar_color == QColor(Qt::transparent))
                setAttribute(Qt::WA_TranslucentBackground, true);
            else if (m_title_bar_color.isValid())
                setStyleSheet(m_style.arg(m_title_bar_color.name()));
            else if (qApp->style()->objectName().startsWith("fusion"))
                setStyleSheet(m_style.arg(qApp->palette().base().color().name()));
            else
                setStyleSheet(m_style.arg("#000000"));
        });

        //Light mode to contrast
        m_min_btn->setIconMode(false);
        m_max_btn->setIconMode(false);
        m_restore_btn->setIconMode(false);
        m_cls_btn->setIconMode(false);

        m_title_widget->setTitleColor(QColor(255, 255, 255), QColor(150, 150, 150));

        setActive(m_active);
    }
    else
    {
        QTimer::singleShot(0, this, [=]{
            if (m_title_bar_color == QColor(Qt::transparent))
                setAttribute(Qt::WA_TranslucentBackground, true);
            else if (m_title_bar_color.isValid())
                setStyleSheet(m_style.arg(m_title_bar_color.name()));
            else if (qApp->style()->objectName().startsWith("fusion"))
                setStyleSheet(m_style.arg(qApp->palette().base().color().name()));
            else
                setStyleSheet(m_style.arg("#FFFFFF"));
        });

        //Dark mode to contrast
        m_min_btn->setIconMode(true);
        m_max_btn->setIconMode(true);
        m_restore_btn->setIconMode(true);
        m_cls_btn->setIconMode(true);

        m_title_widget->setTitleColor(QColor(0, 0, 0), QColor(150, 150, 150));

        setActive(m_active);
    }
}

void TitleBar::setLeftTitleBarWidget(QWidget *widget)
{
    if (m_left_widget_place_holder->layout())
        delete m_left_widget_place_holder->layout();

    m_left_widget = widget;

    if (m_left_widget)
    {
        QGridLayout *layout = new QGridLayout(m_left_widget_place_holder);
        layout->setContentsMargins(0, 0, m_layout_spacing, 0);
        layout->setSpacing(0);
        layout->addWidget(m_left_widget);

        m_left_widget_place_holder->setVisible(true);
        m_left_widget_place_holder->setEnabled(true);
    }
    else
    {
        m_left_widget_place_holder->setVisible(false);
        m_left_widget_place_holder->setEnabled(false);
    }
}

void TitleBar::setRightTitleBarWidget(QWidget *widget)
{
    if (m_right_widget_place_holder->layout())
        delete m_right_widget_place_holder->layout();

    m_right_widget = widget;

    if (m_right_widget)
    {
        QGridLayout *layout = new QGridLayout(m_right_widget_place_holder);
        layout->setContentsMargins(0, 0, m_layout_spacing, 0);
        layout->setSpacing(0);
        layout->addWidget(m_right_widget);

        m_right_widget_place_holder->setVisible(true);
        m_right_widget_place_holder->setEnabled(true);
    }
    else
    {
        m_right_widget_place_holder->setVisible(false);
        m_right_widget_place_holder->setEnabled(false);
    }
}

void TitleBar::setCenterTitleBarWidget(QWidget *widget)
{
    if (m_center_widget_place_holder->layout())
        delete m_center_widget_place_holder->layout();

    m_center_widget = widget;

    if (m_center_widget)
    {
        QGridLayout *layout = new QGridLayout(m_center_widget_place_holder);
        layout->setContentsMargins(0, 0, m_layout_spacing, 0);
        layout->setSpacing(0);
        layout->addWidget(m_center_widget);

        m_center_widget_place_holder->setVisible(true);
        m_center_widget_place_holder->setEnabled(true);
    }
    else
    {
        m_center_widget_place_holder->setVisible(false);
        m_center_widget_place_holder->setEnabled(false);
    }
}

void TitleBar::setCaptionButtonWidth(int width)
{
    m_min_btn->setFixedSize(width, height());
    m_restore_btn->setFixedSize(width, height());
    m_max_btn->setFixedSize(width, height());
    m_cls_btn->setFixedSize(width, height());

    m_caption_buttons->adjustSize();
}

Qt::Alignment TitleBar::titleAlignment()
{
    return m_title_widget->titleAlignment();
}

int TitleBar::captionButtonsWidth()
{
    return m_caption_buttons->width();
}

int TitleBar::leftWidth()
{
    QRect rect;

    rect = rect.united(m_icon_widget->geometry());

    rect = rect.united(m_left_widget_place_holder->geometry());

    return rect.width();
}

int TitleBar::rightWidth()
{
    QRect rect;

    rect = rect.united(m_caption_buttons->geometry());

    rect = rect.united(m_right_widget_place_holder->geometry());

    int width = rect.width() + layoutSpacing();

    return width;
}

int TitleBar::layoutSpacing()
{
    return m_layout_spacing;
}

bool TitleBar::isMinimizedButtonEnabled()
{
    if (!m_gw)
        return false;

    return (m_gw->windowFlags() & Qt::WindowMinimizeButtonHint);
}

bool TitleBar::isMaximizeButtonEnabled()
{
    if (!m_gw)
        return false;

    return (m_gw->windowFlags() & Qt::WindowMaximizeButtonHint);
}

bool TitleBar::isCloseButtonEnabled()
{
    if (!m_gw)
        return false;

    return (m_gw->windowFlags() & Qt::WindowCloseButtonHint);
}

QRect TitleBar::minimizeButtonRect()
{
    if (m_min_btn->isVisible())
        return m_min_btn->geometry();

    return QRect();
}

QRect TitleBar::maximizeButtonRect()
{
    if (m_max_btn->isVisible())
        return m_max_btn->geometry();
    if (m_restore_btn->isVisible())
        return m_restore_btn->geometry();

    return QRect();
}

QRect TitleBar::closeButtonRect()
{
    if (m_cls_btn->isVisible())
        return m_cls_btn->geometry();

    return QRect();
}

void TitleBar::updateWindow()
{
    int left_width = leftWidth();

    int right_width = rightWidth();

    int distance = right_width - left_width;

    int left_distance = 0;

    int right_distance = 0;

    if (distance > 0)
        left_distance = qAbs(distance);
    else
        right_distance = qAbs(distance);

    m_center_spacer_item_left->changeSize(left_distance, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);

    m_center_spacer_item_right->changeSize(right_distance, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);
}

void TitleBar::captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state)
{
    if (!m_caption_buttons->isVisible())
        return;

    switch (state)
    {
        // Hover enter
    case QGoodWindow::CaptionButtonState::MinimizeHoverEnter:
    {
        m_min_btn->setState(QEvent::HoverEnter);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeHoverEnter:
    {
        if (!m_is_maximized)
            m_max_btn->setState(QEvent::HoverEnter);
        else
            m_restore_btn->setState(QEvent::HoverEnter);

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseHoverEnter:
    {
        m_cls_btn->setState(QEvent::HoverEnter);

        break;
    }
        // Hover leave
    case QGoodWindow::CaptionButtonState::MinimizeHoverLeave:
    {
        m_min_btn->setState(QEvent::HoverLeave);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeHoverLeave:
    {
        if (!m_is_maximized)
            m_max_btn->setState(QEvent::HoverLeave);
        else
            m_restore_btn->setState(QEvent::HoverLeave);

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseHoverLeave:
    {
        m_cls_btn->setState(QEvent::HoverLeave);

        break;
    }
        // Mouse button press
    case QGoodWindow::CaptionButtonState::MinimizePress:
    {
        m_min_btn->setState(QEvent::MouseButtonPress);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizePress:
    {
        if (!m_is_maximized)
            m_max_btn->setState(QEvent::MouseButtonPress);
        else
            m_restore_btn->setState(QEvent::MouseButtonPress);

        break;
    }
    case QGoodWindow::CaptionButtonState::ClosePress:
    {
        m_cls_btn->setState(QEvent::MouseButtonPress);

        break;
    }
        // Mouse button release
    case QGoodWindow::CaptionButtonState::MinimizeRelease:
    {
        m_min_btn->setState(QEvent::MouseButtonRelease);

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeRelease:
    {
        if (!m_is_maximized)
            m_max_btn->setState(QEvent::MouseButtonRelease);
        else
            m_restore_btn->setState(QEvent::MouseButtonRelease);

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseRelease:
    {
        m_cls_btn->setState(QEvent::MouseButtonRelease);

        break;
    }
        // Mouse button clicked
    case QGoodWindow::CaptionButtonState::MinimizeClicked:
    {
        Q_EMIT m_min_btn->clicked();

        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeClicked:
    {
        if (!m_is_maximized)
            Q_EMIT m_max_btn->clicked();
        else
            Q_EMIT m_restore_btn->clicked();

        break;
    }
    case QGoodWindow::CaptionButtonState::CloseClicked:
    {
        Q_EMIT m_cls_btn->clicked();

        break;
    }
    default:
        break;
    }
}
