/*
The MIT License (MIT)

Copyright © 2023 Antonio Dias (https://github.com/antonypro)

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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent)
{
    m_settings = new QSettings("settings.ini", QSettings::IniFormat, this);

    m_good_central_widget = new QGoodCentralWidget(this);

    //Contents of the window
    m_label = new QLabel("Hello world!", this);
    m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    //Function that sets the state holder theme
    auto func_theme = [=]{
        qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());
    };

    //React to state holder theme change and apply our dark or light theme to the whole app
    connect(qGoodStateHolder, &QGoodStateHolder::currentThemeChanged, this, [=]{
        if (qGoodStateHolder->isCurrentThemeDark())
            QGoodWindow::setAppDarkTheme();
        else
            QGoodWindow::setAppLightTheme();
    });

    //React to system theme change
    connect(this, &QGoodWindow::systemThemeChanged, this, func_theme);

    //Set the central widget of QGoodCentralWidget
    m_good_central_widget->setCentralWidget(m_label);

    //Set the central widget of QGoodWindow which is QGoodCentralWidget
    setCentralWidget(m_good_central_widget);

    //Set initial theme
    func_theme();

#ifdef QGOODWINDOW
    m_good_central_widget->setUnifiedTitleBarAndCentralWidget(true);

    //Align title to the right side
    m_good_central_widget->setTitleAlignment(Qt::AlignRight);

    //Add widget to the left of the title bar
    m_good_central_widget->setLeftTitleBarWidget(new QPushButton("Hello 1", this));

    QWidget *widget = new QWidget(this);
    widget->setFixedWidth(300);
    QHBoxLayout *layout1 = new QHBoxLayout(widget);
    layout1->setContentsMargins(0, 0, 0, 0);
    layout1->addWidget(new QPushButton("Hello 2", this));
    layout1->addWidget(new QLineEdit(this));

    //Add widget to the center of the title bar
    m_good_central_widget->setCenterTitleBarWidget(widget, true);

    //Add widget to the right of the title bar
    m_good_central_widget->setRightTitleBarWidget(new QPushButton("Hello 3", this));
#endif
}

MainWindow::~MainWindow()
{

}

void MainWindow::adjustSizeLabel()
{
    QFont font = m_label->font();

    QFontMetrics metrics(font);
    QSize size = metrics.size(0, m_label->text());
    qreal scale_width = qreal(m_label->width()) / qreal(size.width());
    qreal scale_height = qreal(m_label->height()) / qreal(size.height());

    qreal scale = qMin(scale_width, scale_height);

    if (qFuzzyIsNull(scale))
        scale = qreal(1);

    font.setPointSizeF(font.pointSizeF() * scale);
    m_label->setFont(font);
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Show:
    case QEvent::Resize:
    {
        QTimer::singleShot(0, this, &MainWindow::adjustSizeLabel);
        break;
    }
    default:
        break;
    }

    return QGoodWindow::event(event);
}

void MainWindow::show()
{
    QTimer::singleShot(0, this, [=]{
        //Restore window geometry
        if (!restoreGeometry(m_settings->value("geometry").toByteArray()))
        {
            //If there is no saved geometry, use a default geometry
            if (QWidget *central_widget = m_good_central_widget->centralWidget())
            {
                resize(qMax(central_widget->minimumSizeHint().width(), 500),
                       qMax(central_widget->minimumSizeHint().height(), 200));
            }

            move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());
        }

        //Call parent show
        QGoodWindow::show();
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    //Save geometry for next open
    m_settings->setValue("geometry", saveGeometry());
}
