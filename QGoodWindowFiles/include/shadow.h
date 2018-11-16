#ifndef SHADOW_H
#define SHADOW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#define SHADOWWIDTH 10
#define COLOR1 QColor(0, 0, 0, 75)
#define COLOR2 QColor(0, 0, 0, 30)
#define COLOR3 QColor(0, 0, 0, 1)

class Shadow : public QWidget
{
    Q_OBJECT
public:
    explicit Shadow(HWND hwnd, QWidget *parent = nullptr);

public slots:
    void showLater(bool show);
    void show();
    void hide();
    void setActive(bool active);

private:
    //Functions
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void paintEvent(QPaintEvent *event);

    //Variables
    HWND m_hwnd;
    QTimer *m_timer;
    bool m_active;
};

#endif // SHADOW_H
