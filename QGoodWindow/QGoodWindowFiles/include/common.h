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

#ifndef COMMON_H
#define COMMON_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "include/qgoodwindow.h"

#ifdef Q_OS_WIN
//\cond HIDDEN_SYMBOLS
inline void sizeDialog(QDialog *dialog, QGoodWindow *parent)
{
    QScreen *screen = parent->windowHandle()->screen();
    QRect windowrect;

    if (!parent->isMinimized() && parent->isVisible())
    {
        windowrect = parent->frameGeometry();
    }
    else
    {
        windowrect = screen->availableGeometry();
    }

    QRect screenrect = screen->availableGeometry();

    QRect finalrect = screenrect.intersected(windowrect);

    QRect newrect = dialog->frameGeometry();

    newrect.adjust(0, parent->titleBarHeight(), 0, 0);

    newrect.moveCenter(finalrect.center());

    dialog->setGeometry(newrect);

    if (dialog->windowIcon().isNull())
        dialog->setWindowIcon(parent->windowIcon());

    dialog->setAttribute(Qt::WA_DontShowOnScreen, true);
    dialog->setAttribute(Qt::WA_DontShowOnScreen, false);

    if (parent->isMinimized())
        parent->showNormal();
}
//\endcond
#endif

#endif // COMMON_H
