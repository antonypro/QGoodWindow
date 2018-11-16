#ifndef MODALWIDGET_H
#define MODALWIDGET_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class ModalWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ModalWidget(HWND hwnd, QWidget *parent = nullptr);

private:
    bool event(QEvent *event);

private:
    HWND hwnd;
    QWidget *child;
};

#endif // MODALWIDGET_H
