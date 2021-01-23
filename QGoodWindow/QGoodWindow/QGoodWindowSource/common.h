/*
The MIT License (MIT)

Copyright © 2018-2021 Antonio Dias

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

#ifndef COMMON_H
#define COMMON_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "QGoodWindowSource/qgoodwindow.h"

#ifdef Q_OS_WIN

#include <windows.h>

#ifndef SM_CXPADDEDBORDER
#define SM_CXPADDEDBORDER 92
#endif

//\cond HIDDEN_SYMBOLS
inline void moveDialog(QDialog *dialog, QGoodWindow *good_window)
{
    const int title_bar_height = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
    const int border_width = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));

    QScreen *screen = good_window->windowHandle()->screen();
    QRect good_window_rect;

    if (!good_window->isMinimized() && good_window->isVisible())
    {
        good_window_rect = good_window->frameGeometry();
    }
    else
    {
        good_window_rect = screen->availableGeometry();
    }

    QRect screen_rect = screen->availableGeometry();

    QRect dialog_rect = dialog->geometry();

    dialog_rect.moveCenter(good_window_rect.center());

    dialog_rect.moveLeft(qMax(dialog_rect.left(), screen_rect.left() + border_width));
    dialog_rect.moveTop(qMax(dialog_rect.top() + good_window->titleBarHeight() / 2, screen_rect.top() + title_bar_height));
    dialog_rect.moveRight(qMin(dialog_rect.right(), screen_rect.right() - border_width));
    dialog_rect.moveBottom(qMin(dialog_rect.bottom(), screen_rect.bottom() - border_width));

    dialog->setGeometry(dialog_rect);

    if (dialog->windowIcon().isNull())
        dialog->setWindowIcon(good_window->windowIcon());

    dialog->setAttribute(Qt::WA_DontShowOnScreen, true);
    dialog->setAttribute(Qt::WA_DontShowOnScreen, false);

    if (good_window->isMinimized())
        good_window->showNormal();
}
//\endcond

#endif

#endif // COMMON_H
