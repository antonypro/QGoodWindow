/*
The MIT License (MIT)

Copyright © 2022 Antonio Dias

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

#include "qgooddialog.h"
#include "qgoodwindow.h"
#include "shadow.h"

QGoodDialog::QGoodDialog(QDialog *dialog, QGoodWindow *child_gw, QGoodWindow *parent_gw) : QObject()
{
    m_dialog = dialog;

    m_child_gw = child_gw;

    m_parent_gw = parent_gw;
}

int QGoodDialog::exec()
{
    if (!m_dialog)
        return QDialog::Rejected;

    if (!m_child_gw)
        return QDialog::Rejected;

    if (!m_parent_gw)
        return QDialog::Rejected;

    m_child_gw->installEventFilter(this);

    m_dialog->installEventFilter(this);

#ifdef Q_OS_WIN
    m_dialog->setWindowModality(Qt::NonModal);
#endif
#ifdef Q_OS_LINUX
    m_child_gw->setWindowModality(Qt::ApplicationModal);
#endif
#ifdef Q_OS_MAC
    m_child_gw->setWindowModality(Qt::WindowModal);
#endif

    if (qobject_cast<QMessageBox*>(m_dialog) != nullptr)
    {
        m_child_gw->setFixedSize(m_child_gw->sizeHint());
    }
    else if (qobject_cast<QInputDialog*>(m_dialog) != nullptr)
    {
        m_child_gw->resize(m_child_gw->sizeHint());
        m_child_gw->setFixedHeight(m_child_gw->sizeHint().height());
    }

    if (!m_dialog->windowTitle().isNull())
        m_child_gw->setWindowTitle(m_dialog->windowTitle());
    else
        m_child_gw->setWindowTitle(qApp->applicationName());

    if (!m_dialog->windowIcon().isNull())
        m_child_gw->setWindowIcon(m_dialog->windowIcon());
    else
        m_child_gw->setWindowIcon(m_parent_gw->windowIcon());

    if (m_parent_gw->isMinimized())
        m_parent_gw->showNormal();

    QRect geom;
    geom.setSize(m_child_gw->size());
    geom.moveCenter(m_parent_gw->frameGeometry().center());
    m_child_gw->setGeometry(geom);

    QTimer::singleShot(0, m_child_gw, &QGoodWindow::show);

    m_loop.exec();

    m_dialog->setParent(nullptr);
    m_child_gw->setParent(nullptr);

    return m_dialog->result();
}

int QGoodDialog::exec(QDialog *dialog, QGoodWindow *child_gw, QGoodWindow *parent_gw)
{
    QGoodDialog good_dialog(dialog, child_gw, parent_gw);
    return good_dialog.exec();
}

bool QGoodDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_child_gw)
    {
        switch (event->type())
        {
        case QEvent::Show:
        {
#ifdef Q_OS_WIN
            HWND hwnd_gw = HWND(m_child_gw->winId());

            for (QWindow *window : qApp->topLevelWindows())
            {
                HWND hwnd = HWND(window->winId());

                if (hwnd == hwnd_gw || IsChild(hwnd_gw, hwnd))
                    continue;

                EnableWindow(hwnd, FALSE);
            }
#endif
#ifdef Q_OS_MAC
            for (QWindow *w : qApp->topLevelWindows())
            {
                if (!w->isVisible())
                    continue;

                if (w == m_child_gw->windowHandle())
                    continue;

                if (w == m_parent_gw->windowHandle())
                    continue;

                if (w == m_dialog->windowHandle())
                    continue;

                m_window_list.append(w);

                w->setModality(Qt::WindowModal);
            }
#endif
            break;
        }
        case QEvent::Close:
        {
            QTimer::singleShot(0, m_parent_gw, &QGoodWindow::activateWindow);

            m_loop.quit();

#ifdef Q_OS_WIN
            for (QWindow *window : qApp->topLevelWindows())
            {
                HWND hwnd = HWND(window->winId());
                EnableWindow(hwnd, TRUE);
            }
#endif
#ifdef Q_OS_MAC
            for (QWindow *w : m_window_list)
            {
                w->setModality(Qt::NonModal);
            }

            m_window_list.clear();
#endif
            break;
        }
        default:
            break;
        }
    }
    else if (watched == m_dialog)
    {
        switch (event->type())
        {
        case QEvent::Show:
        {
            if (m_dialog)
                m_dialog->activateWindow();

            break;
        }
        case QEvent::Hide:
        {
            if (m_child_gw)
                m_child_gw->close();

            break;
        }
        default:
            break;
        }
    }

    return QObject::eventFilter(watched, event);
}
