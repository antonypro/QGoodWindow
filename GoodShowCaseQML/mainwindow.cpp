#include "mainwindow.h"
#ifdef Q_OS_WIN
#include "captionbutton.h"
#endif

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent)
{
#ifdef Q_OS_WIN
    title_bar = new TitleBar(pixelRatio(), this);

    frame = new QFrame(this);

    m_frame_style = QString("QFrame {background-color: %0; border: %1;}");

    m_color_str = "#FFFFFF";

    frame->setStyleSheet(m_frame_style.arg(m_color_str).arg("1px solid #1883D7"));

    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(title_bar);

    connect(title_bar, &TitleBar::showMinimized, this, &MainWindow::showMinimized);
    connect(title_bar, &TitleBar::showNormal, this, &MainWindow::showNormal);
    connect(title_bar, &TitleBar::showMaximized, this, &MainWindow::showMaximized);
    connect(title_bar, &TitleBar::closeWindow, this, &MainWindow::close);

    connect(this, &MainWindow::windowTitleChanged, [=](const QString &title){
        title_bar->setTitle(title);
    });

    connect(this, &MainWindow::windowIconChanged, [=](const QIcon &icon){
        if (!icon.isNull())
            title_bar->setIcon(icon.pixmap(16, 16));
    });

    QQuickWidget *view_widget = new QQuickWidget(frame);
    view_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view_widget->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    layout->addWidget(view_widget);

    setCentralWidget(frame);

    setMargins(qRound(30 * pixelRatio()), qRound(30 * pixelRatio()), 0, qRound(36 * 3 * pixelRatio()));

    QPixmap pix = QPixmap(1, 1);
    pix.fill(Qt::yellow);

    QWinTaskbarButton *button = new QWinTaskbarButton(this);
    button->setWindow(windowHandle());
    button->setOverlayIcon(pix);

    QWinTaskbarProgress *progress = button->progress();
    progress->setVisible(true);
    progress->setValue(50);

    connect(this, &MainWindow::windowTitleChanged, [=](const QString &title){
        QTimer::singleShot(0, [=]{
            title_bar->setTitle(title);
        });
    });

    connect(this, &MainWindow::windowIconChanged, [=](const QIcon &icon){
        if (!icon.isNull())
            title_bar->setIcon(icon.pixmap(qRound(16 * pixelRatio()), qRound(16 * pixelRatio())));
    });
#else
    QQuickWidget *view_widget = new QQuickWidget(this);
    view_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view_widget->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    setCentralWidget(view_widget);
#endif
    resize(640, 480);

    move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());

#ifdef Q_OS_WIN
    QShortcut *shortcut = new QShortcut(QKeySequence("CTRL+W"), this);

    connect(shortcut, &QShortcut::activated, [=]{
        title_bar->setDarkMode(!title_bar->dark());

        if (!title_bar->dark())
            m_color_str = "#FFFFFF";
        else
            m_color_str = "#000000";

        frame->setStyleSheet(m_frame_style.arg(m_color_str).arg(isMaximized() ? "none" : "1px solid #1883D7"));
    });
#endif

    QPixmap p = QPixmap(1, 1);
    p.fill(Qt::red);

    setWindowIcon(p);

    setWindowTitle("Good QML Demo - Press CTRL+W on Windows to switch theme!");
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int result = QMessageBox::question(parentForModal(), "Close window", "Are you sure to close?");

    if (result != QMessageBox::Yes)
        event->ignore();
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
    MSG *msg = reinterpret_cast<MSG*>(message);
    qDebug() << __FUNCTION__ << msg->message;
#endif

    return QGoodWindow::nativeEvent(eventType, message, result);
}

bool MainWindow::event(QEvent *event)
{
#ifdef Q_OS_WIN
    switch (event->type())
    {
    case QEvent::Show:
    case QEvent::WindowStateChange:
    {
        frame->setStyleSheet(m_frame_style.arg(m_color_str).arg(isMaximized() ? "none" : "1px solid #1883D7"));

        title_bar->setMaximized(isMaximized());

        break;
    }
    case QEvent::WindowActivate:
    {
        frame->setStyleSheet(m_frame_style.arg(m_color_str).arg(isMaximized() ? "none" : "1px solid #1883D7"));

        title_bar->setActive(true);

        break;
    }
    case QEvent::WindowDeactivate:
    {
        frame->setStyleSheet(m_frame_style.arg(m_color_str).arg(isMaximized() ? "none" : "1px solid #AAAAAA"));

        title_bar->setActive(false);

        break;
    }
    case QEvent::Resize:
    case QEvent::Hide:
    case QEvent::Move:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseMove:
    case QEvent::Wheel:
//        qDebug() << __FUNCTION__ << event;
        break;
    default:
        break;
    }
#endif

    return QMainWindow::event(event);
}
