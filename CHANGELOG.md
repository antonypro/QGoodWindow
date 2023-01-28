## *Version 2.2.1:*

- Fixed bug that not shows the window icon set by the application.

## *Version 2.2:*

- Added possibility to place title also on center or right side of the title bar on QGoodCentralWidget.
- Added possibility to place widgets on center of the title bar on QGoodCentralWidget.
- Added possibility to unify title bar and central widget on QGoodCentralWidget.
- Added possibility to build QGoodWindow as shared or static library. [#9](https://github.com/antonypro/QGoodWindow/issues/9)
- Copied behavior of QMainWindow that always center QDialog to parent on QDialog show. [#23](https://github.com/antonypro/QGoodWindow/issues/23)
- Fixed bug that uses wrong geometry on saveGeometry() making window grows when geometry is restored. [#21](https://github.com/antonypro/QGoodWindow/issues/21)
- Fixed bug that makes title bar widgets unable to receive focus.
- Fixed bug that makes impossible compiling when no_qgoodwindow is set. [#22](https://github.com/antonypro/QGoodWindow/issues/22)
- Fixed bug that show buttons on full screen restore on macOS.
- Fixed bug that prevent from showing correct layout spacing on macOS.
- Other bug fixes.

## *Version 2.1:*

- Added CMake build system support. [#18](https://github.com/antonypro/QGoodWindow/issues/18)
- Added QGoodCentralWidget as a separate module to make easier manage QGoodWindow.
- Added qGoodStateHolder macro to QGoodWindow module to handle dark and light modes application wide.
- Added Light and Dark fusion themes to QGoodWindow module.
- Added possibility to modal QDialog also have customized title bar and borders.
- Fixed problems when window receives focus back from modal QDialog.
- isActiveWindow() returns true also if active window is a children of the window.
- Prevent from resize widgets while the window is not is visible.
- Improved resize border on Linux, that respects the current window stack.
- Implemented functions to save and restore window state as is in QMainWindow. [#15](https://github.com/antonypro/QGoodWindow/issues/15)
- Fixed bug that prevents process to exit on last window close on Windows.
- API windowFlags() returns correct flags for the QGoodWindow.
- QMessageBox uses the up-to-date icons on Windows.
- Fixed warning while building on Linux related to GTK.
- Fixed bug that prevents interact with the window after moving on Qt 6.
- Fixed bug with high DPI scaling in Qt 6. [#19](https://github.com/antonypro/QGoodWindow/issues/19)
- Other bug fixes.

## *Version 2.0:*

- Added support for Qt 6. [#3](https://github.com/antonypro/QGoodWindow/issues/3)
- Added support for Windows 11 rounded corners.
- Added support for Windows 11 caption buttons show text and Snap window on maximize button.
- Added API to see if OS is in Dark mode or not.
- Added SIGNAL to notify when OS theme changes from Dark and Light modes.
- Added API to show when borders must be drawn by OS.
- Added support for central widget self resize, like resize with QSizeGrip.
- Fixed focus bug when window gains and loses focus on Windows. [#8](https://github.com/antonypro/QGoodWindow/issues/8)
- Other bug fixes.

## *Version 1.1:*

- Added mask possibility to QGoodWindow margins. [#1](https://github.com/antonypro/QGoodWindow/issues/1)
- Fixed QWidget focus bugs when the window gets and lose focus.
- Move QGoodWindow to another screen changes it's QScreen information, and full screen works correctly on multiple monitors on Windows. [#4](https://github.com/antonypro/QGoodWindow/issues/4)
- Fixed bugs when Qt::AA_NativeWindows is set or QWidget::winId() is called on Windows preventing the window from being moved. [#5](https://github.com/antonypro/QGoodWindow/issues/5)
- Fixed bug that sometimes not shows the correct mouse cursor on Linux.
- Fixed bug that on some Linux distributions shows undesired rounded corners on the window.
- QGoodWindow::frameGeometry() now reports the correct window geometry on Windows even when maximized.
- Added qgoodwindow CONFIG for qmake.
- The API QGoodWindow::setClearColor() was removed and now clear color should be passed directly to class constructor.
- Other bug fixes.

## *Version 1.0:*

- Added support for Linux and macOS.
- Fixed bug that crashes application on QCoreApplication::exit(). [#2](https://github.com/antonypro/QGoodWindow/issues/2)
- Added support for multiple windows on the same process.
- Other minor bug fixes.
