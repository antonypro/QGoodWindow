#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGoodWindow>
#ifdef Q_OS_WIN
#include "titlebar.h"
#endif

class CaptionButton;

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //Functions
    void closeEvent(QCloseEvent *event);
    bool event(QEvent *event);

    //Variables
#ifdef Q_OS_WIN
    TitleBar *title_bar;
    QFrame *frame;
    QString m_frame_style;
    QString m_color_str;
#endif
};

#endif // MAINWINDOW_H
