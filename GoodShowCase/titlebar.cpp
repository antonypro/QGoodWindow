#include "titlebar.h"

TitleBar::TitleBar(qreal pixel_ratio, QWidget *parent) : QFrame(parent)
{
    style = QString("TitleBar {background-color: %0; border: none;}");

    setFixedHeight(29);

    iconwidget = new IconWidget(this);
    iconwidget->setFixedWidth(29);

    titlewidget = new TitleWidget(this);

    minbtn = new CaptionButton(CaptionButton::IconType::Minimize, pixel_ratio, this);
    minbtn->setFixedSize(36, 29);

    restorebtn = new CaptionButton(CaptionButton::IconType::Restore, pixel_ratio, this);
    restorebtn->setFixedSize(36, 29);

    maxbtn = new CaptionButton(CaptionButton::IconType::Maximize, pixel_ratio, this);
    maxbtn->setFixedSize(36, 29);

    clsbtn = new CaptionButton(CaptionButton::IconType::Close, pixel_ratio, this);
    clsbtn->setFixedSize(36, 29);

    connect(minbtn, &CaptionButton::clicked, this, &TitleBar::showMinimized);
    connect(restorebtn, &CaptionButton::clicked, this, &TitleBar::showNormal);
    connect(maxbtn, &CaptionButton::clicked, this, &TitleBar::showMaximized);
    connect(clsbtn, &CaptionButton::clicked, this, &TitleBar::closeWindow);

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->setSpacing(0);
    hlayout->setMargin(0);

    hlayout->addWidget(iconwidget);
    hlayout->addWidget(titlewidget);
    hlayout->addWidget(minbtn);
    hlayout->addWidget(restorebtn);
    hlayout->addWidget(maxbtn);
    hlayout->addWidget(clsbtn);

    m_active = true;

    m_is_maximized = false;

    setDarkMode(false);
}

void TitleBar::setTitle(const QString &title)
{
    titlewidget->setText(title);
}

void TitleBar::setIcon(const QPixmap &icon)
{
    iconwidget->setPixmap(icon);
}

void TitleBar::setActive(bool active)
{
    m_active = active;

    iconwidget->setActive(active);
    titlewidget->setActive(active);
    minbtn->setActive(active);
    restorebtn->setActive(active);
    maxbtn->setActive(active);
    clsbtn->setActive(active);
}

void TitleBar::setMaximized(bool maximized)
{
    m_is_maximized = maximized;

    if (maximized)
    {
        restorebtn->setVisible(true);
        maxbtn->setVisible(false);
    }
    else
    {
        restorebtn->setVisible(false);
        maxbtn->setVisible(true);
    }
}

bool TitleBar::dark()
{
    return m_dark;
}

void TitleBar::setDarkMode(bool dark)
{
    m_dark = dark;

    if (dark)
    {
        setStyleSheet(style.arg("#000000"));
        update();

        //Light mode to contrast
        minbtn->setIconMode(false);
        maxbtn->setIconMode(false);
        restorebtn->setIconMode(false);
        clsbtn->setIconMode(false);

        titlewidget->setTitleColor(QColor(255, 255, 255), QColor(150, 150, 150));

        setActive(m_active);
    }
    else
    {
        setStyleSheet(style.arg("#FFFFFF"));
        update();

        //Dark mode to contrast
        minbtn->setIconMode(true);
        maxbtn->setIconMode(true);
        restorebtn->setIconMode(true);
        clsbtn->setIconMode(true);

        titlewidget->setTitleColor(QColor(0, 0, 0), QColor(150, 150, 150));

        setActive(m_active);
    }
}
