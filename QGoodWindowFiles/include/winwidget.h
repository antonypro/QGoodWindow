#ifndef WINWIDGET_H
#define WINWIDGET_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "qwinwidget.h"
#include <windowsx.h>

class WinWidget : public QWinWidget
{
    Q_OBJECT
public:
    explicit WinWidget(HWND parent = 0);

    //Functions
    void setMargins(int title_bar_height, int icon_width, int left, int right);

private:
    //Functions
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

    //Variables
    int m_title_bar_height;
    int m_icon_width;
    int m_left;
    int m_right;
};

#endif // WINWIDGET_H
