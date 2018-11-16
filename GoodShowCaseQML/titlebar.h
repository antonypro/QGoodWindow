#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "iconwidget.h"
#include "titlewidget.h"
#include "captionbutton.h"

class TitleBar : public QFrame
{
    Q_OBJECT
public:
    explicit TitleBar(qreal pixel_ratio, QWidget *parent = nullptr);

signals:
    void showMinimized();
    void showNormal();
    void showMaximized();
    void closeWindow();

public slots:
    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);
    void setActive(bool active);
    void setMaximized(bool maximized);
    bool dark();
    void setDarkMode(bool dark);

private:
    IconWidget *iconwidget;
    TitleWidget *titlewidget;
    CaptionButton *minbtn;
    CaptionButton *restorebtn;
    CaptionButton *maxbtn;
    CaptionButton *clsbtn;
    QString style;
    bool m_active;
    bool m_is_maximized;
    bool m_dark;
};

#endif // TITLEBAR_H
