#ifndef CAPTIONBUTTON_H
#define CAPTIONBUTTON_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class CaptionButton : public QWidget
{
    Q_OBJECT
public:
    enum class IconType
    {
        Minimize,
        Restore,
        Maximize,
        Close
    };

    explicit CaptionButton(IconType type, qreal pixelRatio, QWidget *parent = nullptr);

signals:
    void clicked();

public slots:
    void setIconMode(bool icon_dark);
    void setActive(bool active_window);

private:
    //Functions
    void setColors();
    void drawIcons();
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);
    bool underMouse();

    //Variables
    QPixmap m_inactive_icon;
    QPixmap m_active_icon;
    QPixmap m_last_icon;
    QPixmap m_close_icon_hover;

    QColor m_normal;
    QColor m_hover;
    QColor m_pressed;

    QPixmap m_current_icon;
    QColor m_current_color;

    IconType m_type;
    qreal m_pixel_ratio;
    bool m_is_pressed;
    bool m_icon_dark;
};

#endif // CAPTIONBUTTON_H
