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

#ifndef CAPTIONBUTTON_H
#define CAPTIONBUTTON_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class CaptionButton : public QWidget
{
    Q_OBJECT
public:
    enum class IconType
    {
        Minimize,
        Restore,
        Maximize,
        Close
    };

    explicit CaptionButton(IconType type, qreal pixelRatio, QWidget *parent = nullptr);

signals:
    void clicked();

public slots:
    void setIconMode(bool icon_dark);
    void setActive(bool active_window);

private:
    //Functions
    void setColors();
    void drawIcons();
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);
    bool underMouse();

    //Variables
    QPixmap m_inactive_icon;
    QPixmap m_active_icon;
    QPixmap m_last_icon;
    QPixmap m_close_icon_hover;

    QColor m_normal;
    QColor m_hover;
    QColor m_pressed;

    QPixmap m_current_icon;
    QColor m_current_color;

    IconType m_type;
    qreal m_pixel_ratio;
    bool m_is_pressed;
    bool m_icon_dark;
};

#endif // CAPTIONBUTTON_H
