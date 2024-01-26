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

#ifndef MACOSNATIVE_H
#define MACOSNATIVE_H

#include "notification.h"

extern Notification notification;

//\cond HIDDEN_SYMBOLS
namespace macOSNative
{
enum class StyleType
{
    NoState,
    Disabled,
    Fullscreen
};

class Style
{
public:
    StyleType m_style = StyleType::NoState;
    bool m_is_native_caption_buttons_visible = true;
    bool m_is_dialog = false;
};

void registerThemeChangeNotification();
void registerNotification(const char *notification_name, long wid);
void unregisterNotification();

inline void handleNotification(const char *notification_name, long wid)
{
    notification.notification(notification_name, wid);
}

void setStyle(long winid, Style *style);

const char *themeName();
void frameGeometry(long wid, int *x, int *y, int *w, int *h);
void titleBarButtonsRect(long wid, int *x, int *y, int *w, int *h);
}
//\endcond

#endif // MACOSNATIVE_H
