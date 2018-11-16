#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGoodWindow>

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_H
