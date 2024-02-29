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

#include "notification.h"
#include "qgoodwindow.h"
#include "macosnative.h"

Notification::Notification()
{

}

Notification::~Notification()
{
    unregisterNotification();
}

void Notification::addWindow(void *ptr)
{
    m_ptr_list.append(ptr);
}

void Notification::removeWindow(void *ptr)
{
    m_ptr_list.removeAll(ptr);
}

void Notification::registerNotification(const QByteArray &name, WId wid)
{
    macOSNative::registerNotification(name.constData(), long(wid));
}

void Notification::unregisterNotification()
{
    macOSNative::unregisterNotification();
}

void Notification::notification(const char *notification_name, long wid)
{
    const QByteArray notification = QByteArray(notification_name);

    for (void *ptr : m_ptr_list)
    {
        QGoodWindow *gw = static_cast<QGoodWindow*>(ptr);

        if (wid == 0)
        {
            gw->notificationReceiver(notification);
        }
        else if (gw->winId() == WId(wid))
        {
            gw->notificationReceiver(notification);
            break;
        }
    }
}
