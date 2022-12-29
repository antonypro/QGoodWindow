/*
The MIT License (MIT)

Copyright © 2022 Antonio Dias

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QGoodWindow>
#include <QGoodCentralWidget>

#include "ui_centralwidget.h"

class CentralWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr) : QMainWindow(parent), ui(new Ui::CentralWidget)
    {
        ui->setupUi(this);
    }

    ~CentralWidget()
    {
        delete ui;
    }

    Ui::CentralWidget *ui;
};

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void themeChange();

private:
    //Functions
#ifdef QT_VERSION_QT5
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif
#ifdef QT_VERSION_QT6
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result);
#endif
    void closeEvent(QCloseEvent *event);

    //Variables
    QGoodCentralWidget *m_good_central_widget;
    CentralWidget *m_central_widget;
};

#endif // MAINWINDOW_H
