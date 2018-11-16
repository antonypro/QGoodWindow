#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = nullptr);

public slots:
    void setText(const QString &text);
    void setActive(bool active);
    void setTitleColor(const QColor &active_color, const QColor &inactive_color);

private:
    //Functions
    void paintEvent(QPaintEvent *event);

    //Variables
    QString m_title;
    bool m_active;
    QColor m_active_color;
    QColor m_inactive_color;
};

#endif // TITLEWIDGET_H
