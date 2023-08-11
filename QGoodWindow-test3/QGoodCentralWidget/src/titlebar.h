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

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QGoodWindow>
#include "iconwidget.h"
#include "titlewidget.h"
#include "captionbutton.h"

//\cond HIDDEN_SYMBOLS
class TitleBar : public QFrame
{
    Q_OBJECT
public:
    explicit TitleBar(QGoodWindow *gw, QWidget *parent = nullptr);

signals:
    void showMinimized();
    void showNormal();
    void showMaximized();
    void closeWindow();

public slots:
    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);
    void setActive(bool active);
    void setTitleAlignment(const Qt::Alignment &alignment);
    void setMaximized(bool maximized);
    void setTheme();
    void setLeftTitleBarWidget(QWidget *widget);
    void setRightTitleBarWidget(QWidget *widget);
    void setCenterTitleBarWidget(QWidget *widget);
    void setCaptionButtonWidth(int width);
    Qt::Alignment titleAlignment();
    int captionButtonsWidth();
    int leftWidth();
    int rightWidth();
    int layoutSpacing();
    bool isMinimizedButtonEnabled();
    bool isMaximizeButtonEnabled();
    bool isCloseButtonEnabled();
    QRect minimizeButtonRect();
    QRect maximizeButtonRect();
    QRect closeButtonRect();
    void updateWindow();
    void captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state);

private:
    QPointer<QGoodWindow> m_gw;
    IconWidget *m_icon_widget;
    TitleWidget *m_title_widget;
    QWidget *m_caption_buttons;
    CaptionButton *m_min_btn;
    CaptionButton *m_restore_btn;
    CaptionButton *m_max_btn;
    CaptionButton *m_cls_btn;
    QPointer<QWidget> m_left_widget;
    QPointer<QWidget> m_right_widget;
    QPointer<QWidget> m_center_widget;
    QPointer<QWidget> m_left_widget_place_holder;
    QPointer<QWidget> m_right_widget_place_holder;
    QPointer<QWidget> m_center_widget_place_holder;
    QSpacerItem *m_center_spacer_item_left;
    QSpacerItem *m_center_spacer_item_right;
    int m_layout_spacing;
    QString style;
    bool m_active;
    bool m_is_maximized;
    QColor m_title_bar_color;

    friend class QGoodCentralWidget;
    friend class TitleWidget;
};
//\endcond

#endif // TITLEBAR_H
