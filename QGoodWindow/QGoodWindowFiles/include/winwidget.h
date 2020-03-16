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

#ifndef WINWIDGET_H
#define WINWIDGET_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "qwinwidget.h"
#include <windowsx.h>

//\cond HIDDEN_SYMBOLS
class WinWidget : public QWinWidget
{
    Q_OBJECT
public:
    explicit WinWidget(HWND parent_hwnd, QWidget *parent = nullptr);

    //Functions
    void setMargins(int title_bar_height, int icon_width, int left, int right);

private:
    //Functions
    bool event(QEvent *event);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

    //Variables
    QWidget *m_parent;
    int m_title_bar_height;
    int m_icon_width;
    int m_left;
    int m_right;
};
//\endcond

#endif // WINWIDGET_H
