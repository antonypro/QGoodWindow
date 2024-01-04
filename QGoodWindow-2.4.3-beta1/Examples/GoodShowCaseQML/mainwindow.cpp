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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent, QColor("#303030"))
{
    m_good_central_widget = new QGoodCentralWidget(this);

    m_quick_widget = new QQuickWidget(this);
    m_quick_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quick_widget->rootContext()->setContextObject(this);
    m_quick_widget->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

#ifdef QGOODWINDOW
    m_good_central_widget->setUnifiedTitleBarAndCentralWidget(true);

    m_good_central_widget->setIconVisible(false);
    m_good_central_widget->setTitleVisible(false);
    m_good_central_widget->setCaptionButtonsVisible(false);
    m_good_central_widget->setTitleBarHeight(30);
    m_good_central_widget->setIconWidth(30);

    m_good_central_widget->setCaptionButtonType(QGoodCentralWidget::CaptionButtonType::Custom);

    connect(this, &QGoodWindow::windowTitleChanged, this, [=](const QString &title){
        if (!m_quick_widget)
            return;

        m_quick_widget->rootObject()->setProperty("windowTitle", title);
    });

    connect(this, &QGoodWindow::windowIconChanged, this, [=](const QIcon &icon){
        if (!m_quick_widget)
            return;

        const int pix_size = 16;

        QPixmap pix = icon.pixmap(pix_size, pix_size);
        m_quick_widget->rootObject()->setProperty("windowIcon", loadPixmapAsBase64(pix));

        QPixmap pix_grayed = loadGrayedPixmap(pix);
        m_quick_widget->rootObject()->setProperty("windowIconGrayed", loadPixmapAsBase64(pix_grayed));
    });

    connect(this, &QGoodWindow::captionButtonStateChanged,
            this, &MainWindow::captionButtonStateChangedPrivate);

    qGoodStateHolder->setCurrentThemeDark(true);

    QGoodWindow::setAppDarkTheme();
#endif

    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::Key_F), this);

    connect(shortcut1, &QShortcut::activated, this, [=]{
        if (!isFullScreen())
            showFullScreen();
        else
            showNormal();
    });

    m_good_central_widget->setCentralWidget(m_quick_widget);
    setCentralWidget(m_good_central_widget);

    QPixmap p = QPixmap(1, 1);
    p.fill(Qt::red);

    setWindowIcon(p);
    setWindowTitle("Good Window - Press F to toggle fullscreen!");

    resize(640, 480);
    move(qApp->primaryScreen()->availableGeometry().center() - rect().center());

#ifdef Q_OS_WIN
#ifdef QT_VERSION_QT5
    QTimer::singleShot(0, this, [=]{
        QPixmap p = QPixmap(1, 1);
        p.fill(Qt::yellow);

        QWinTaskbarButton *button = new QWinTaskbarButton(this);
        button->setWindow(windowHandle());
        button->setOverlayIcon(p);

        QWinTaskbarProgress *progress = button->progress();
        progress->setVisible(true);
        progress->setValue(50);
    });
#endif
#endif

#ifdef QGOODWINDOW
    m_quick_widget->rootObject()->setProperty("isTitleBarVisible", true);
#else
    m_good_central_widget->setTitleBarVisible(false);
    m_quick_widget->rootObject()->setProperty("isTitleBarVisible", false);
#endif
}

MainWindow::~MainWindow()
{

}

QPixmap MainWindow::loadGrayedPixmap(const QPixmap &pix)
{
    QImage tmp = pix.toImage();

    for (int y = 0; y < tmp.height(); y++)
    {
        QRgb *pixel_ptr = reinterpret_cast<QRgb*>(tmp.scanLine(y));

        for (int x = 0; x < tmp.width(); x++)
        {
            QRgb pixel = pixel_ptr[x];

            int gray = qGray(pixel);

            int alpha = qAlpha(pixel);

            QRgb rgba = qRgba(gray, gray, gray, alpha);

            pixel_ptr[x] = rgba;
        }
    }

    QPixmap pix_grayed = QPixmap::fromImage(tmp);

    return pix_grayed;
}

QString MainWindow::loadPixmapToString(const QString &path)
{
    if (m_pix_str_hash.contains(path))
        return m_pix_str_hash.value(path);

    QPixmap pix = QIcon(path).pixmap(10, 10);

    QString pix_str = loadPixmapAsBase64(pix);

    m_pix_str_hash.insert(path, pix_str);

    return pix_str;
}

QString MainWindow::loadPixmapAsBase64(const QPixmap &pix)
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG", 0);
    buffer.close();

    QString pix_str = QString("data:image/png;base64,%0").arg(QLatin1String(data.toBase64()));

    return pix_str;
}

void MainWindow::captionButtonStateChangedPrivate(const QGoodWindow::CaptionButtonState &state)
{
    if (!m_quick_widget)
        return;

    switch (state)
    {
    case CaptionButtonState::MinimizeHoverEnter:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "minBtn"), Q_ARG(QVariant, "hover_enter"));
        break;
    }
    case CaptionButtonState::MinimizeHoverLeave:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "minBtn"), Q_ARG(QVariant, "hover_leave"));
        break;
    }
    case CaptionButtonState::MinimizePress:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "minBtn"), Q_ARG(QVariant, "button_press"));
        break;
    }
    case CaptionButtonState::MinimizeRelease:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "minBtn"), Q_ARG(QVariant, "button_release"));
        break;
    }
    case CaptionButtonState::MaximizeHoverEnter:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "maxBtn"), Q_ARG(QVariant, "hover_enter"));
        break;
    }
    case CaptionButtonState::MaximizeHoverLeave:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "maxBtn"), Q_ARG(QVariant, "hover_leave"));
        break;
    }
    case CaptionButtonState::MaximizePress:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "maxBtn"), Q_ARG(QVariant, "button_press"));
        break;
    }
    case CaptionButtonState::MaximizeRelease:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "maxBtn"), Q_ARG(QVariant, "button_release"));
        break;
    }
    case CaptionButtonState::CloseHoverEnter:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "clsBtn"), Q_ARG(QVariant, "hover_enter"));
        break;
    }
    case CaptionButtonState::CloseHoverLeave:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "clsBtn"), Q_ARG(QVariant, "hover_leave"));
        break;
    }
    case CaptionButtonState::ClosePress:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "clsBtn"), Q_ARG(QVariant, "button_press"));
        break;
    }
    case CaptionButtonState::CloseRelease:
    {
        QMetaObject::invokeMethod(m_quick_widget->rootObject(), "setButtonState",
                                  Q_ARG(QVariant, "clsBtn"), Q_ARG(QVariant, "button_release"));
        break;
    }
    case CaptionButtonState::MinimizeClicked:
    {
        QTimer::singleShot(0, this, &QGoodWindow::showMinimized);

        break;
    }
    case CaptionButtonState::MaximizeClicked:
    {
        if (!isMaximized())
            QTimer::singleShot(0, this, &QGoodWindow::showMaximized);
        else
            QTimer::singleShot(0, this, &QGoodWindow::showNormal);

        break;
    }
    case CaptionButtonState::CloseClicked:
    {
        QTimer::singleShot(0, this, &QGoodWindow::close);

        break;
    }
    default:
        break;
    }
}

bool MainWindow::event(QEvent *event)
{
#ifdef QGOODWINDOW
    switch (event->type())
    {
    case QEvent::Resize:
    {
        if (isFullScreen())
            break;

        QTimer::singleShot(0, this, [=]{
            QRect rect = titleBarRect();

            QRegion min_mask = rect.adjusted(rect.width() - 30 * 3, -30 * 2, 0, 0);
            QRegion max_mask = rect.adjusted(rect.width() - 30 * 2, -30, 0, 0);
            QRegion cls_mask = rect.adjusted(rect.width() - 30, 0, 0, 0);

            QPainterPath path;
            path.addRoundedRect(5, 5, 20, 20, 20, 20);

            {
                QRegion mask = QRegion(path.toFillPolygon().toPolygon());
                min_mask = mask.translated(min_mask.boundingRect().x(), 0);
            }
            {
                QRegion mask = QRegion(path.toFillPolygon().toPolygon());
                max_mask = mask.translated(max_mask.boundingRect().x(), 0);
            }
            {
                QRegion mask = QRegion(path.toFillPolygon().toPolygon());
                cls_mask = mask.translated(cls_mask.boundingRect().x(), 0);
            }

            setMinimizeMask(min_mask);
            setMaximizeMask(max_mask);
            setCloseMask(cls_mask);
        });

        break;
    }
    case QEvent::ActivationChange:
    {
        if (!m_quick_widget)
            break;

        bool is_active = isActiveWindow();
        m_quick_widget->rootObject()->setProperty("isActive", is_active);

        break;
    }
    case QEvent::WindowStateChange:
    {
        QWindowStateChangeEvent *change_event = static_cast<QWindowStateChangeEvent*>(event);

        bool is_maximized = isMaximized();

        if (m_quick_widget)
            m_quick_widget->rootObject()->setProperty("isMaximized", is_maximized);

        if (!change_event->oldState().testFlag(Qt::WindowFullScreen) &&
                !windowState().testFlag(Qt::WindowFullScreen))
            break;

        bool is_full_screen = isFullScreen();
        m_good_central_widget->setTitleBarVisible(!is_full_screen);

        if (m_quick_widget)
            m_quick_widget->rootObject()->setProperty("isTitleBarVisible", !is_full_screen);

        break;
    }
    default:
        break;
    }
#endif
    return QGoodWindow::event(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgbox(this);
    msgbox.setIcon(QMessageBox::Question);
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox.setDefaultButton(QMessageBox::No);
    msgbox.setText("Are you sure to close?");

    int result = QGoodCentralWidget::execDialogWithWindow(&msgbox, this, m_good_central_widget);

    if (result != QMessageBox::Yes)
        event->ignore();
}
