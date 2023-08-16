/*
The MIT License (MIT)

Copyright © 2018-2023 Antonio Dias (https://github.com/antonypro)

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
#include <QtQml>
#include <QtQuick>
#include <QtQuickWidgets>
#include <QGoodWindow>
#include <QGoodCentralWidget>

#ifdef Q_OS_WIN
#ifdef QT_VERSION_QT5
#include <QtWinExtras>
#endif
#endif

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public Q_SLOTS:
    QString loadPixmapToString(const QString &path);

private Q_SLOTS:
    QPixmap loadGrayedPixmap(const QPixmap &pix);
    QString loadPixmapAsBase64(const QPixmap &pix);
    void captionButtonStateChangedPrivate(const QGoodWindow::CaptionButtonState &state);

private:
    //Functions
    bool event(QEvent *event);
    void closeEvent(QCloseEvent *event);

    //Variables
    QGoodCentralWidget *m_good_central_widget;
    QPointer<QQuickWidget> m_quick_widget;
    QHash<QString, QString> m_pix_str_hash;
};

#endif // MAINWINDOW_H
