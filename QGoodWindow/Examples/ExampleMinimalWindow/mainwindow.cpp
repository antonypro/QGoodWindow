/*
The MIT License (MIT)

Copyright © 2018-2022 Antonio Dias

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
    m_good_central_widget = new QGoodCentralWidget(this);

    //Contents of the window
    QLabel *label = new QLabel("Hello world!", this);
    QFont font = label->font();
    font.setPointSize(72);
    label->setFont(font);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    //Function that sets the state holder theme
    auto func_theme = [=]{
        qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());
    };

    //React to state holder theme change and apply our dark or light theme to the whole app
    connect(qGoodStateHolder, &QGoodStateHolder::currentThemeChanged, this, [=]{
        if (qGoodStateHolder->isCurrentThemeDark())
            setAppDarkTheme();
        else
            setAppLightTheme();
    });

    //React to system theme change
    connect(this, &QGoodWindow::systemThemeChanged, this, func_theme);

    //Set the central widget of QGoodCentralWidget
    m_good_central_widget->setCentralWidget(label);

    //Set the central widget of QGoodWindow which is QGoodCentralWidget
    setCentralWidget(m_good_central_widget);

    //Set initial theme
    func_theme();
}

MainWindow::~MainWindow()
{

}
