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

#ifndef CAPTIONBUTTON_H
#define CAPTIONBUTTON_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtSvg>

//\cond HIDDEN_SYMBOLS
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

    explicit CaptionButton(IconType type, QWidget *parent = nullptr);
    ~CaptionButton();

Q_SIGNALS:
    void clicked();

public Q_SLOTS:
    void setIconMode(bool icon_dark);
    void setActive(bool is_active);
    void setState(int state);

private:
    //Functions
    QPixmap loadSVG(const QString &svg_path, int w, int h);
    void paintIcons(const QPixmap &pix_in, bool dark,
                    QPixmap *pix_active_out, QPixmap *pix_inactive_out);
    void drawIcons();
    void setColors();
    void paintEvent(QPaintEvent *event);

    //Variables
    QPixmap m_inactive_icon;
    QPixmap m_active_icon;

    QPixmap m_close_icon_hover;

    QColor m_normal;
    QColor m_hover;
    QColor m_pressed;

    IconType m_type;
    bool m_is_active;
    bool m_is_under_mouse;
    bool m_is_pressed;
    bool m_icon_dark;
};
//\endcond

#endif // CAPTIONBUTTON_H
