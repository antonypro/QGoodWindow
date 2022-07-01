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

- Added mask possibility to `QGoodWindow` margins. [#1](https://github.com/antonypro/QGoodWindow/issues/1)
- Fixed `QWidget` focus bugs when the window gets and lose focus.
- Move `QGoodWindow` to another screen changes it's `QScreen` information, and full screen works correctly on multiple monitors on Windows. [#4](https://github.com/antonypro/QGoodWindow/issues/4)
- Fixed bugs when `Qt::AA_NativeWindows` is set or `QWidget::winId()` is called on Windows preventing the window from being moved. [#5](https://github.com/antonypro/QGoodWindow/issues/5)
- Fixed bug that sometimes not shows the correct mouse cursor on Linux.
- Fixed bug that on some Linux distributions shows undesired rounded corners on the window.
- `QGoodWindow::frameGeometry()` now reports the correct window geometry on Windows even when maximized.
- Added `qgoodwindow` `CONFIG` for `qmake`.
- The API `QGoodWindow::setClearColor()` was removed and now clear color should be passed directly to class constructor.
- Other bug fixes.

## *Version 1.0:*

- Added support for Linux and macOS.
- Fixed bug that crashes application on `QCoreApplication::exit()`. [#2](https://github.com/antonypro/QGoodWindow/issues/2)
- Added support for multiple windows on the same process.
- Other minor bug fixes.
