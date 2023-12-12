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

#include "qgooddialog.h"
#include "qgoodwindow.h"

#ifdef Q_OS_MAC
#include "macosnative.h"
#endif

namespace QGoodDialogUtils
{
static bool m_dialog_is_visible = false;
}

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

    if (QGoodDialogUtils::m_dialog_is_visible)
        return QDialog::Rejected;

    QGoodDialogUtils::m_dialog_is_visible = true;

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

#if defined Q_OS_WIN || defined Q_OS_LINUX
    bool is_message_box = qobject_cast<QMessageBox*>(m_dialog);
    bool is_input_dialog = qobject_cast<QInputDialog*>(m_dialog);

    auto func_center = [=]{
        QScreen *parent_screen = m_parent_gw->windowHandle()->screen();

        qreal pixel_ratio = parent_screen->devicePixelRatio();
        QRect screen_geom = parent_screen->availableGeometry();
        screen_geom.moveTop(qFloor(screen_geom.top() / pixel_ratio));
        screen_geom.moveLeft(qFloor(screen_geom.left() / pixel_ratio));

        QRect child_geom;
        child_geom.setSize(m_child_gw->size());
        child_geom.moveCenter(m_parent_gw->geometry().center());

        if (child_geom.left() < screen_geom.left())
            child_geom.moveLeft(screen_geom.left());
        else if (child_geom.right() > screen_geom.right())
            child_geom.moveRight(screen_geom.right());

        if (child_geom.top() < screen_geom.top())
            child_geom.moveTop(screen_geom.top());
        else if (child_geom.bottom() > screen_geom.bottom())
            child_geom.moveBottom(screen_geom.bottom());

        m_child_gw->setGeometry(child_geom);
    };

    auto func_fixed_size = [=]{
        if (is_message_box || is_input_dialog)
        {
            m_child_gw->setFixedSize(m_child_gw->sizeHint());
        }
        else
        {
            if (m_dialog->minimumSize() == m_dialog->maximumSize())
            {
                m_child_gw->setFixedSize(m_child_gw->sizeHint());
            }
            else
            {
                m_child_gw->setMinimumSize(m_dialog->minimumSize());
                m_child_gw->setMaximumSize(m_dialog->maximumSize());
            }
        }
    };
#endif

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

#ifdef Q_OS_MAC
    QTimer::singleShot(0, m_child_gw, [=]{
        bool visible = m_child_gw->isNativeCaptionButtonsVisibleOnMac();
        m_child_gw->setNativeCaptionButtonsVisibleOnMac(visible);
        m_child_gw->show();
        m_child_gw->setNativeCaptionButtonsVisibleOnMac(visible);
    });
#else
    func_fixed_size();
    func_center();

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), m_child_gw);
    connect(shortcut, &QShortcut::activated, m_dialog, &QDialog::reject);

    QTimer::singleShot(0, m_child_gw, [=]{
        m_child_gw->show();
#ifdef Q_OS_LINUX
        func_fixed_size();
        func_center();
#endif
    });
#endif

    m_loop.exec();

    m_dialog->setParent(nullptr);
    m_child_gw->setParent(nullptr);

    QGoodDialogUtils::m_dialog_is_visible = false;

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

            for (QWindow *w : qApp->topLevelWindows())
            {
                //Prevent problems with fullscreen mode.
                if (w->type() == Qt::Window)
                    continue;

                HWND hwnd = HWND(w->winId());

                if (hwnd == hwnd_gw || IsChild(hwnd_gw, hwnd))
                    continue;

                if (m_window_list.contains(w))
                    continue;

                m_window_list.append(w);

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

            m_parent_gw->setMacOSStyle(int(macOSNative::StyleType::Disabled));
#endif
            break;
        }
        case QEvent::Close:
        {
            QTimer::singleShot(0, m_parent_gw, &QGoodWindow::activateWindow);

            m_loop.quit();

#ifdef Q_OS_WIN
            for (QWindow *w : m_window_list)
            {
                HWND hwnd = HWND(w->winId());
                EnableWindow(hwnd, TRUE);
            }

            m_window_list.clear();
#endif
#ifdef Q_OS_MAC
            for (QWindow *w : m_window_list)
            {
                w->setModality(Qt::NonModal);
            }

            m_window_list.clear();

            QTimer::singleShot(500, this, [=]{
                m_parent_gw->setMacOSStyle(int(macOSNative::StyleType::NoState));
            });
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
