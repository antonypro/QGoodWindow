#include "include/modalwidget.h"

ModalWidget::ModalWidget(HWND hwnd, QWidget *parent) : QWidget(parent), hwnd(hwnd), child(nullptr)
{

}

bool ModalWidget::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::ChildAdded:
    {
        QChildEvent *childevent = static_cast<QChildEvent*>(event);
        child = qobject_cast<QWidget*>(childevent->child());

        break;
    }
    case QEvent::ChildRemoved:
    {
        delete this;

        return false;
    }
    case QEvent::WindowBlocked:
    {
        if (child && child->isModal())
            EnableWindow(hwnd, FALSE);

        break;
    }
    case QEvent::WindowUnblocked:
    {
        if (child && child->isModal())
            EnableWindow(hwnd, TRUE);

        break;
    }
    default:
        break;
    }

    return QWidget::event(event);
}
