/*
The MIT License (MIT)

Copyright © 2018-2024 Antonio Dias (https://github.com/antonypro)

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

#include "qgoodcentralwidget.h"

#ifdef QGOODWINDOW
#include "titlebar.h"
#define BORDERCOLOR QColor(24, 131, 215)
#endif

QGoodCentralWidget::QGoodCentralWidget(QGoodWindow *gw) : QWidget(gw)
{
    m_gw = gw;

    m_gw->installEventFilter(this);

    m_central_widget_place_holder = new QWidget(this);

#ifdef QGOODWINDOW
    m_update_later_timer = new QTimer(this);
    connect(m_update_later_timer, &QTimer::timeout, this, &QGoodCentralWidget::updateWindowLater);
    m_update_later_timer->setSingleShot(true);
    m_update_later_timer->setInterval(0);

    m_left_widget_transparent_for_mouse = false;
    m_right_widget_transparent_for_mouse = false;
    m_center_widget_transparent_for_mouse = false;

    m_frame_style = QString("QFrame#GoodFrame {%0}");

    m_active_border_color = BORDERCOLOR;

    m_caption_button_width = 36;

    m_title_bar = new TitleBar(m_gw, this, this);
    m_title_bar->setCaptionButtonWidth(m_caption_button_width);

    m_title_bar->installEventFilter(this);

    connect(m_gw, &QGoodWindow::captionButtonStateChanged, m_title_bar, &TitleBar::captionButtonStateChanged);

    connect(m_title_bar, &TitleBar::showMinimized, m_gw, &QGoodWindow::showMinimized);
    connect(m_title_bar, &TitleBar::showNormal, m_gw, &QGoodWindow::showNormal);
    connect(m_title_bar, &TitleBar::showMaximized, m_gw, &QGoodWindow::showMaximized);
    connect(m_title_bar, &TitleBar::closeWindow, m_gw, &QGoodWindow::close);

    connect(m_gw, &QGoodWindow::windowTitleChanged, m_title_bar, [=](const QString &title){
        m_title_bar->setTitle(title);
    });

    connect(m_gw, &QGoodWindow::windowIconChanged, m_title_bar, [=](const QIcon &icon){
        if (!icon.isNull())
        {
            const int pix_size = 16;
            m_title_bar->setIcon(icon.pixmap(pix_size, pix_size));
        }
    });

    m_draw_borders = !QGoodWindow::shouldBordersBeDrawnBySystem();

#ifdef Q_OS_MAC
    connect(m_gw, &QGoodWindow::captionButtonsVisibilityChangedOnMacOS,
            this, &QGoodCentralWidget::updateCaptionButtonsState);
#endif

    m_title_bar_visible = true;
    m_caption_buttons_visible = true;
    m_title_visible = true;
    m_icon_visibility_type = IconVisibilityType::IconOnLeftOfWindow;

#ifdef Q_OS_MAC
    m_caption_buttons_type = CaptionButtonsType::Native;
#else
    m_caption_buttons_type = CaptionButtonsType::CentralWidget;
#endif

    m_frame = new QFrame(this);
    m_frame->setObjectName("GoodFrame");

    setUnifiedTitleBarAndCentralWidget(false);

    QVBoxLayout *central_layout = new QVBoxLayout(this);
    central_layout->setContentsMargins(0, 0, 0, 0);
    central_layout->setSpacing(0);
    central_layout->addWidget(m_frame);
#else
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    main_layout->addWidget(m_central_widget_place_holder);
#endif
}

QGoodCentralWidget::~QGoodCentralWidget()
{

}

int QGoodCentralWidget::execDialogWithWindow(QDialog *dialog, QGoodWindow *parent_gw,
                                             QGoodCentralWidget *base_gcw,
                                             QWidget *left_title_bar_widget,
                                             QWidget *right_title_bar_widget,
                                             bool title_visible, bool icon_visible)
{
#ifdef QGOODWINDOW
    dialog->setWindowFlags(Qt::Widget);

    QGoodWindow *gw = new QGoodWindow(parent_gw);
    gw->resize(dialog->size());
    gw->setAttribute(Qt::WA_DeleteOnClose);

    QGoodCentralWidget *gcw = new QGoodCentralWidget(gw);
    gcw->setCentralWidget(dialog);

    gcw->setLeftTitleBarWidget(left_title_bar_widget);
    gcw->setRightTitleBarWidget(right_title_bar_widget);

    if (base_gcw)
    {
#ifdef Q_OS_MAC
        gcw->setCaptionButtonsType(base_gcw->captionButtonsType());
#endif
        if (base_gcw->titleBarColor() != QColor(Qt::transparent))
            gcw->setTitleBarColor(base_gcw->titleBarColor());
        gcw->setActiveBorderColor(base_gcw->activeBorderColor());
        gcw->setTitleAlignment(base_gcw->titleAlignment());
    }

    gcw->setTitleVisible(title_visible);

    if (icon_visible)
    {
#ifndef Q_OS_MAC
        gcw->setIconVisibility(IconVisibilityType::IconOnLeftOfWindow);
#else
        gcw->setIconVisibility(IconVisibilityType::IconOnLeftOfTitle);
#endif
    }
    else
    {
        gcw->setIconVisibility(IconVisibilityType::IconHidden);
    }

    gw->setCentralWidget(gcw);

    return QGoodWindow::execDialog(dialog, gw, parent_gw);
#else
    Q_UNUSED(parent_gw)
    Q_UNUSED(base_gcw)
    Q_UNUSED(left_title_bar_widget)
    Q_UNUSED(right_title_bar_widget)
    Q_UNUSED(title_visible)
    Q_UNUSED(icon_visible)
    return dialog->exec();
#endif
}

void QGoodCentralWidget::setCaptionButtonsType(const CaptionButtonsType &type)
{
#ifdef QGOODWINDOW
    m_caption_buttons_type = type;
    updateCaptionButtonsState();
#else
    Q_UNUSED(type)
#endif
}

void QGoodCentralWidget::setUnifiedTitleBarAndCentralWidget(bool unified)
{
#ifdef QGOODWINDOW
    if (m_frame->layout())
        delete m_frame->layout();

    if (!unified)
    {
        m_unified_title_bar_and_central_widget = false;
        setTitleBarColor(QColor());

        QVBoxLayout *main_layout = new QVBoxLayout(m_frame);
        main_layout->setContentsMargins(0, 0, 0, 0);
        main_layout->setSpacing(0);
        main_layout->addWidget(m_title_bar);
        main_layout->addWidget(m_central_widget_place_holder);
    }
    else
    {
        setTitleBarColor(QColor(Qt::transparent));
        m_unified_title_bar_and_central_widget = true;

        QStackedLayout *main_layout = new QStackedLayout(m_frame);
        main_layout->setStackingMode(QStackedLayout::StackAll);
        main_layout->setContentsMargins(0, 0, 0, 0);
        main_layout->setSpacing(0);
        main_layout->addWidget(m_title_bar);
        main_layout->addWidget(m_central_widget_place_holder);
    }
#else
    Q_UNUSED(unified)
#endif
}

void QGoodCentralWidget::setTitleBarMask(const QRegion &mask)
{
#ifdef QGOODWINDOW
    m_title_bar_mask = mask;
    updateWindow();
#else
    Q_UNUSED(mask)
#endif
}

QWidget *QGoodCentralWidget::setLeftTitleBarWidget(QWidget *widget, bool transparent_for_mouse)
{
#ifdef QGOODWINDOW
    QWidget *return_widget = m_title_bar_left_widget;

    m_title_bar_left_widget = widget;

    m_left_widget_transparent_for_mouse = transparent_for_mouse;

    m_title_bar->setLeftTitleBarWidget(m_title_bar_left_widget);

    updateWindow();

    return return_widget;
#else
    Q_UNUSED(widget)
    Q_UNUSED(transparent_for_mouse)
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::setRightTitleBarWidget(QWidget *widget, bool transparent_for_mouse)
{
#ifdef QGOODWINDOW
    QWidget *return_widget = m_title_bar_right_widget;

    m_title_bar_right_widget = widget;

    m_right_widget_transparent_for_mouse = transparent_for_mouse;

    m_title_bar->setRightTitleBarWidget(m_title_bar_right_widget);

    updateWindow();

    return return_widget;
#else
    Q_UNUSED(widget)
    Q_UNUSED(transparent_for_mouse)
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::setCenterTitleBarWidget(QWidget *widget, bool transparent_for_mouse)
{
#ifdef QGOODWINDOW
    QWidget *return_widget = m_title_bar_center_widget;

    m_title_bar_center_widget = widget;

    m_center_widget_transparent_for_mouse = transparent_for_mouse;

    m_title_bar->setCenterTitleBarWidget(m_title_bar_center_widget);

    updateWindow();

    return return_widget;
#else
    Q_UNUSED(widget)
    Q_UNUSED(transparent_for_mouse)
    return nullptr;
#endif
}

void QGoodCentralWidget::setCentralWidget(QWidget *widget)
{
    if (!m_central_widget_place_holder)
        return;

    m_central_widget = widget;

    if (m_central_widget_place_holder->layout())
        delete m_central_widget_place_holder->layout();

    QGridLayout *layout = new QGridLayout(m_central_widget_place_holder);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_central_widget);
}

void QGoodCentralWidget::setTitleAlignment(const Qt::Alignment &alignment)
{
#ifdef QGOODWINDOW
    m_title_bar->setTitleAlignment(alignment);
#else
    Q_UNUSED(alignment)
#endif
}

void QGoodCentralWidget::setTitleBarColor(const QColor &color)
{
#ifdef QGOODWINDOW
    if (m_unified_title_bar_and_central_widget)
        return;

    m_title_bar_color = color;
    m_title_bar->m_title_bar_color = m_title_bar_color;
    m_title_bar->setTheme();
#else
    Q_UNUSED(color)
#endif
}

void QGoodCentralWidget::setActiveBorderColor(const QColor &color)
{
#ifdef QGOODWINDOW
    if (color.isValid())
        m_active_border_color = color;
    else
        m_active_border_color = BORDERCOLOR;

    updateWindow();
#else
    Q_UNUSED(color)
#endif
}

void QGoodCentralWidget::setTitleBarVisible(bool visible)
{
#ifdef QGOODWINDOW
    m_title_bar_visible = visible;
    updateCaptionButtonsState();
    updateWindow();
#else
    Q_UNUSED(visible)
#endif
}

void QGoodCentralWidget::setCaptionButtonsVisible(bool visible)
{
#ifdef QGOODWINDOW
    m_caption_buttons_visible = visible;
    updateCaptionButtonsState();

    if (!m_caption_buttons_visible)
    {
        m_gw->setMinimizeMask(QRegion());
        m_gw->setMaximizeMask(QRegion());
        m_gw->setCloseMask(QRegion());
    }

    updateWindow();
#else
    Q_UNUSED(visible)
#endif
}

void QGoodCentralWidget::setIconVisibility(const IconVisibilityType &type)
{
#ifdef QGOODWINDOW
    m_icon_visibility_type = type;

    updateWindow();
#else
    Q_UNUSED(type)
#endif
}

//DEPRECATED
void QGoodCentralWidget::setTitleVisible(bool visible)
{
#ifdef QGOODWINDOW
    m_title_visible = visible;
    m_title_bar->m_title_widget->setVisible(m_title_visible);
    m_title_bar->m_title_widget->setEnabled(m_title_visible);
    updateWindow();
#else
    Q_UNUSED(visible)
#endif
}
//DEPRECATED

//DEPRECATED
void QGoodCentralWidget::setIconVisible(bool visible)
{
#ifdef QGOODWINDOW
    setIconVisibility(visible ? IconVisibilityType::IconOnLeftOfWindow : IconVisibilityType::IconHidden);
#else
    Q_UNUSED(visible)
#endif
}
//DEPRECATED

//DEPRECATED
void QGoodCentralWidget::setIconWidth(int width)
{
#ifdef QGOODWINDOW
    m_gw->setIconWidth(width);
    updateWindow();
#else
    Q_UNUSED(width)
#endif
}
//DEPRECATED

void QGoodCentralWidget::setTitleBarHeight(int height)
{
#ifdef QGOODWINDOW
    m_title_bar->setFixedHeight(height);
    m_title_bar->setCaptionButtonWidth(m_caption_button_width);
    updateWindow();
#else
    Q_UNUSED(height)
#endif
}

void QGoodCentralWidget::setCaptionButtonWidth(int width)
{
#ifdef QGOODWINDOW
    m_caption_button_width = width;
    m_title_bar->setCaptionButtonWidth(m_caption_button_width);
    updateWindow();
#else
    Q_UNUSED(width)
#endif
}

QGoodCentralWidget::CaptionButtonsType QGoodCentralWidget::captionButtonsType() const
{
#ifdef QGOODWINDOW
    return m_caption_buttons_type;
#else
    return CaptionButtonsType::Invalid;
#endif
}

bool QGoodCentralWidget::isUnifiedTitleBarAndCentralWidget() const
{
#ifdef QGOODWINDOW
    return m_unified_title_bar_and_central_widget;
#else
    return false;
#endif
}

QWidget *QGoodCentralWidget::leftTitleBarWidget() const
{
#ifdef QGOODWINDOW
    return m_title_bar_left_widget;
#else
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::rightTitleBarWidget() const
{
#ifdef QGOODWINDOW
    return m_title_bar_right_widget;
#else
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::centerTitleBarWidget() const
{
#ifdef QGOODWINDOW
    return m_title_bar_center_widget;
#else
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::centralWidget() const
{
    return m_central_widget;
}

Qt::Alignment QGoodCentralWidget::titleAlignment() const
{
#ifdef QGOODWINDOW
    return m_title_bar->titleAlignment();
#else
    return Qt::Alignment(0);
#endif
}

QColor QGoodCentralWidget::titleBarColor() const
{
#ifdef QGOODWINDOW
    return m_title_bar_color;
#else
    return QColor();
#endif
}

QColor QGoodCentralWidget::activeBorderColor() const
{
#ifdef QGOODWINDOW
    return m_active_border_color;
#else
    return QColor();
#endif
}

bool QGoodCentralWidget::isTitleBarVisible() const
{
#ifdef QGOODWINDOW
    return m_title_bar->isVisible();
#else
    return true;
#endif
}

bool QGoodCentralWidget::isCaptionButtonsVisible() const
{
#ifdef QGOODWINDOW
    return m_caption_buttons_visible;
#else
    return true;
#endif
}

bool QGoodCentralWidget::isTitleVisible() const
{
#ifdef QGOODWINDOW
    return m_title_visible;
#else
    return true;
#endif
}

QGoodCentralWidget::IconVisibilityType QGoodCentralWidget::iconVisibility() const
{
#ifdef QGOODWINDOW
    return m_icon_visibility_type;
#else
    return IconVisibilityType::IconHidden;
#endif
}

//DEPRECATED
bool QGoodCentralWidget::isIconVisible() const
{
#ifdef QGOODWINDOW
    return (m_icon_visibility_type == IconVisibilityType::IconOnLeftOfWindow ||
            m_icon_visibility_type == IconVisibilityType::IconOnLeftOfTitle);
#else
    return true;
#endif
}
//DEPRECATED

//DEPRECATED
int QGoodCentralWidget::iconWidth() const
{
#ifdef QGOODWINDOW
    return m_gw->iconWidth();
#else
    return 0;
#endif
}
//DEPRECATED

int QGoodCentralWidget::titleBarHeight() const
{
#ifdef QGOODWINDOW
    return m_title_bar->height();
#else
    return 0;
#endif
}

int QGoodCentralWidget::captionButtonWidth() const
{
#ifdef QGOODWINDOW
    return m_caption_button_width;
#else
    return 0;
#endif
}

void QGoodCentralWidget::updateWindow()
{
#ifdef QGOODWINDOW
    updateWindowNow();
    m_update_later_timer->start();
#endif
}

void QGoodCentralWidget::updateWindowNow()
{
#ifdef QGOODWINDOW
#if defined Q_OS_WIN || defined Q_OS_LINUX
    bool window_no_state = m_gw->windowState().testFlag(Qt::WindowNoState);
    bool draw_borders = m_draw_borders;

    if (m_title_bar)
    {
        QRegion mask;

        if (draw_borders && window_no_state)
        {
#ifdef Q_OS_WIN
            const int radius = 0;
#else
            const int radius = 8;
#endif

            QBitmap bmp(m_title_bar->size());
            bmp.clear();

            QPainter painter;
            if (!bmp.isNull() && painter.begin(&bmp))
            {
                painter.setRenderHints(QPainter::Antialiasing);
                painter.setBrush(Qt::color1);
                painter.drawRoundedRect(m_title_bar->rect(), radius, radius);
                painter.fillRect(m_title_bar->rect().adjusted(0, m_title_bar->height() - radius, radius, radius), Qt::color1);
                painter.end();
            }

            mask = bmp;
        }

        m_title_bar->setMask(mask);
    }
#endif
#endif
}

void QGoodCentralWidget::updateWindowLater()
{
#ifdef QGOODWINDOW
    if (!m_gw)
        return;

    if (!m_gw->isVisible() || m_gw->isMinimized())
        return;

    bool window_active = m_gw->isActiveWindow();
    bool window_no_state = m_gw->windowState().testFlag(Qt::WindowNoState);
    bool draw_borders = m_draw_borders;
    bool is_maximized = m_gw->isMaximized();
    bool is_full_screen = m_gw->isFullScreen();
    int icon_width = m_gw->iconWidth();

    if (is_full_screen)
        icon_width = 0;
    else if (m_icon_visibility_type == IconVisibilityType::IconHidden)
        icon_width = 0;
    else if (m_icon_visibility_type == IconVisibilityType::IconOnLeftOfWindow)
        icon_width = m_title_bar->m_icon_widget->width();

    QString border_str = "none;";

    if (draw_borders && window_no_state)
    {
        border_str = "border: 1px solid #00000000;";
    }
#ifdef Q_OS_WIN
    else
    {
        if (window_active)
            m_gw->setNativeBorderColorOnWindows(m_active_border_color);
        else
            m_gw->setNativeBorderColorOnWindows(QColor("#AAAAAA"));
    }
#endif

#ifdef Q_OS_LINUX
    border_str.append("border-radius: 8px;");
#endif

    m_frame->setStyleSheet(m_frame_style.arg(border_str));

    m_title_bar->setMaximized(is_maximized && !is_full_screen);

    m_title_bar->setVisible(m_title_bar_visible && !is_full_screen);

    m_title_bar->setActive(window_active);

    m_gw->setIconWidth(icon_width);

    m_title_bar->updateWindow();

    QTimer::singleShot(100, this, &QGoodCentralWidget::updateWindowMask);
#endif
}

void QGoodCentralWidget::updateWindowMask()
{
#ifdef QGOODWINDOW
    if (!m_gw)
        return;

    if (!m_gw->isVisible() || m_gw->isMinimized())
        return;

    bool is_full_screen = m_gw->isFullScreen();
    int title_bar_width = m_title_bar->width();
    int title_bar_height = m_title_bar->height();

    if (!is_full_screen)
    {
        m_gw->setTitleBarHeight(m_title_bar_visible ? title_bar_height : 0);

        QRegion left_mask;
        QRegion right_mask;
        QRegion center_mask;

        if (m_title_bar_left_widget)
        {
            QWidgetList list;

            if (!m_left_widget_transparent_for_mouse)
                list.append(m_title_bar_left_widget);

            list.append(m_title_bar_left_widget->findChildren<QWidget*>());

            for (QWidget *widget : list)
            {
                if (!widget->testAttribute(Qt::WA_TransparentForMouseEvents))
                {
                    if (!widget->mask().isNull())
                    {
                        left_mask += widget->mask().translated(m_title_bar->m_left_widget_place_holder->pos());
                    }
                    else
                    {
                        QRect geom = widget->geometry();

                        if (geom.width() > m_title_bar_left_widget->width())
                            geom.setWidth(m_title_bar_left_widget->width());

                        left_mask += geom.translated(m_title_bar->m_left_widget_place_holder->pos());
                    }
                }
            }
        }

        if (m_title_bar_right_widget)
        {
            QWidgetList list;

            if (!m_right_widget_transparent_for_mouse)
                list.append(m_title_bar_right_widget);

            list.append(m_title_bar_right_widget->findChildren<QWidget*>());

            for (QWidget *widget : list)
            {
                if (!widget->testAttribute(Qt::WA_TransparentForMouseEvents))
                {
                    if (!widget->mask().isNull())
                    {
                        right_mask += widget->mask().translated(m_title_bar->m_right_widget_place_holder->pos());
                    }
                    else
                    {
                        QRect geom = widget->geometry();

                        if (geom.width() > m_title_bar_right_widget->width())
                            geom.setWidth(m_title_bar_right_widget->width());

                        right_mask += geom.translated(m_title_bar->m_right_widget_place_holder->pos());
                    }
                }
            }
        }

        if (m_title_bar_center_widget)
        {
            QWidgetList list;

            if (!m_center_widget_transparent_for_mouse)
                list.append(m_title_bar_center_widget);

            list.append(m_title_bar_center_widget->findChildren<QWidget*>());

            for (QWidget *widget : list)
            {
                if (!widget->testAttribute(Qt::WA_TransparentForMouseEvents))
                {
                    if (!widget->mask().isNull())
                    {
                        center_mask += widget->mask().translated(m_title_bar->m_center_widget_place_holder->pos());
                    }
                    else
                    {
                        QRect geom = widget->geometry();

                        if (geom.width() > m_title_bar_center_widget->width())
                            geom.setWidth(m_title_bar_center_widget->width());

                        center_mask += geom.translated(m_title_bar->m_center_widget_place_holder->pos());
                    }
                }
            }
        }

        QRegion title_bar_mask;

        if (!m_title_bar_mask.isEmpty())
            title_bar_mask = m_title_bar_mask;
        else
            title_bar_mask -= m_gw->titleBarRect();

        title_bar_mask += left_mask;
        title_bar_mask += center_mask;
        title_bar_mask += right_mask;

        m_gw->setTitleBarMask(title_bar_mask);

        if (m_caption_buttons_visible)
        {
            QRect min_rect = m_title_bar->minimizeButtonRect();
            QRect max_rect = m_title_bar->maximizeButtonRect();
            QRect cls_rect = m_title_bar->closeButtonRect();

            min_rect.moveLeft(title_bar_width - cls_rect.width() - max_rect.width() - min_rect.width());
            max_rect.moveLeft(title_bar_width - cls_rect.width() - max_rect.width());
            cls_rect.moveLeft(title_bar_width - cls_rect.width());

            m_gw->setMinimizeMask(min_rect);
            m_gw->setMaximizeMask(max_rect);
            m_gw->setCloseMask(cls_rect);
        }
    }
#endif
}

void QGoodCentralWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

#ifdef QGOODWINDOW
#if defined Q_OS_WIN || defined Q_OS_LINUX
    bool window_active = m_gw->isActiveWindow();
    bool window_no_state = m_gw->windowState().testFlag(Qt::WindowNoState);
    bool draw_borders = m_draw_borders;

    if (draw_borders)
    {
        QColor color = QColor(0, 0, 0, 0);

        if (window_no_state)
        {
            color = window_active ? m_active_border_color : QColor("#AAAAAA");
        }

        QRegion mask;

        if (window_no_state)
        {
#ifdef Q_OS_WIN
            const int radius = 0;
#else
            const int radius = 8;
#endif

            QBitmap bmp(size());
            bmp.clear();

            QPainter painter;
            if (!bmp.isNull() && painter.begin(&bmp))
            {
                painter.setRenderHints(QPainter::Antialiasing);
                painter.setBrush(Qt::color1);
                painter.drawRoundedRect(rect().adjusted(1, 1, -2, -2), radius, radius);
                painter.end();
            }

            mask = bmp;
        }

        QPainter painter;
        if (painter.begin(this))
        {
            QPainterPath path;
            path.addRect(rect());
            path.addRegion(mask);
            painter.fillPath(path, color);
            painter.end();
        }
    }
#endif
#endif
}

bool QGoodCentralWidget::eventFilter(QObject *watched, QEvent *event)
{
#ifdef QGOODWINDOW
    if (watched == m_gw)
    {
        switch (event->type())
        {
        case QEvent::Show:
        case QEvent::Resize:
        case QEvent::WindowStateChange:
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        {
            updateWindow();
            break;
        }
        default:
            break;
        }
    }
    else if (watched == m_title_bar)
    {
        switch (event->type())
        {
        case QEvent::Show:
        case QEvent::Resize:
        {
            updateWindow();
            break;
        }
        default:
            break;
        }
    }
#endif
    return QWidget::eventFilter(watched, event);
}

bool QGoodCentralWidget::event(QEvent *event)
{
#ifdef QGOODWINDOW
#ifdef Q_OS_LINUX
    switch (event->type())
    {
    case QEvent::Show:
    case QEvent::Resize:
    {
        QTimer::singleShot(0, this, [=]{
            if (!m_gw)
                return;

            if (!m_central_widget_place_holder)
                return;

            QRegion mask;

            if (m_gw->isVisible() && m_gw->windowState().testFlag(Qt::WindowNoState))
            {
                const int radius = 8;

                QBitmap bmp(m_central_widget_place_holder->size());
                bmp.clear();

                QPainter painter;
                if (!bmp.isNull() && painter.begin(&bmp))
                {
                    painter.setRenderHints(QPainter::Antialiasing);
                    painter.setBrush(Qt::color1);
                    painter.drawRoundedRect(m_central_widget_place_holder->rect().adjusted(1, 1, -1, -1), radius, radius);
                    painter.end();
                }

                mask = bmp;
            }

            m_central_widget_place_holder->setMask(mask);
        });

        break;
    }
    default:
        break;
    }
#endif
#ifdef Q_OS_MAC
    switch (event->type())
    {
    case QEvent::Show:
    {
        updateCaptionButtonsState();
        break;
    }
    default:
        break;
    }
#endif
#endif

    return QWidget::event(event);
}

void QGoodCentralWidget::updateCaptionButtonsState()
{
#ifdef QGOODWINDOW
#ifdef Q_OS_MAC
    if (!m_title_bar_visible)
    {
        m_title_bar->m_caption_buttons->setVisible(false);
        m_title_bar->m_caption_buttons->setEnabled(false);

        if (m_gw->isNativeCaptionButtonsVisibleOnMac())
            m_gw->setNativeCaptionButtonsVisibleOnMac(false);

        return;
    }

    switch (m_caption_buttons_type)
    {
    case CaptionButtonsType::Custom:
    {
        m_title_bar->m_caption_buttons->setVisible(false);
        m_title_bar->m_caption_buttons->setEnabled(false);

        if (m_gw->isNativeCaptionButtonsVisibleOnMac())
            m_gw->setNativeCaptionButtonsVisibleOnMac(false);

        break;
    }
    case CaptionButtonsType::CentralWidget:
    {
        m_title_bar->m_caption_buttons->setVisible(true);
        m_title_bar->m_caption_buttons->setEnabled(true);

        if (m_gw->isNativeCaptionButtonsVisibleOnMac())
            m_gw->setNativeCaptionButtonsVisibleOnMac(false);

        break;
    }
    case CaptionButtonsType::Native:
    {
        m_title_bar->m_caption_buttons->setVisible(false);
        m_title_bar->m_caption_buttons->setEnabled(false);

        if (!m_gw->isNativeCaptionButtonsVisibleOnMac())
            m_gw->setNativeCaptionButtonsVisibleOnMac(true);

        break;
    }
    default:
        break;
    }

    if (m_gw->isNativeCaptionButtonsVisibleOnMac())
    {
        QRect rect = m_gw->titleBarButtonsRectOnMacOS();
        m_title_bar->m_left_margin_widget_place_holder->setFixedSize(
                    rect.x() * 2 + rect.width(), rect.y() + rect.height());
        setIconVisibility(IconVisibilityType::IconOnLeftOfTitle);
    }
    else
    {
        m_title_bar->m_left_margin_widget_place_holder->setFixedSize(0, 0);
        setIconVisibility(IconVisibilityType::IconOnLeftOfWindow);
    }
#else
    m_title_bar->m_caption_buttons->setVisible(m_caption_buttons_visible);
    m_title_bar->m_caption_buttons->setEnabled(m_caption_buttons_visible);
#endif
#endif
}
