#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class IconWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IconWidget(QWidget *parent = nullptr);

public slots:
    void setPixmap(const QPixmap &pixmap);
    void setActive(bool active);

private:
    //Functions
    void paintEvent(QPaintEvent *event);

    //Variables
    QPixmap m_pixmap;
    QPixmap m_grayed_pixmap;
    bool m_active;
};

#endif // ICONWIDGET_H
