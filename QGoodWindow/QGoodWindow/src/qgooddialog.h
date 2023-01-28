/*
The MIT License (MIT)

Copyright © 2022-2023 Antonio Dias (https://github.com/antonypro)

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

#ifndef QGOODDIALOG_H
#define QGOODDIALOG_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class QGoodWindow;

//\cond HIDDEN_SYMBOLS
class QGoodDialog : public QObject
{
    Q_OBJECT
private:
    explicit QGoodDialog(QDialog *dialog, QGoodWindow *child_gw, QGoodWindow *parent_gw);

public:
    static int exec(QDialog *dialog, QGoodWindow *child_gw, QGoodWindow *parent_gw);

private:
    //Functions
    int exec();
    bool eventFilter(QObject *watched, QEvent *event);

    //Variables
    QEventLoop m_loop;
    QPointer<QDialog> m_dialog;
    QPointer<QGoodWindow> m_child_gw;
    QPointer<QGoodWindow> m_parent_gw;
#ifdef Q_OS_MAC
    QWindowList m_window_list;
#endif
};
//\endcond

#endif // QGOODDIALOG_H
