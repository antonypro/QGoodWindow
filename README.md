# QGoodWindow
*Border less window implementation for Qt 5 and Qt 6 on Windows, Linux and macOS.*

Version 2.2

**QGoodWindow** in dark mode on Windows 11:
![](img/qgoodwindow.png)

**QGoodWindow** gives to your application full control of the window while keeps native OS window behavior, like minimize effect.

**QGoodWindow** project is split in two modules, the  **QGoodWindow** and **QGoodCentralWidget**.

**QGoodWindow** provides the core of the project and can be used without **QGoodCentralWidget**, however **QGoodCentralWidget** offers a ready to use window with icon, title, caption buttons and a easy way to add widgets to the title bar.

**QGoodWindow** Usage:

*CMake:*

For use **QGoodWindow** add to your `CMakeLists.txt`:

    include(path_to/QGoodWindow/QGoodWindow.cmake)

For use **QGoodCentralWidget** add to your `CMakeLists.txt` after  **QGoodWindow**:

    include(path_to/QGoodCentralWidget/QGoodCentralWidget.cmake)

*QMake:*

For use **QGoodWindow** add to your `.pro`:

    include(path_to/QGoodWindow/QGoodWindow.pri)

For use **QGoodCentralWidget** add to your `.pro` after  **QGoodWindow**:

    include(path_to/QGoodCentralWidget/QGoodCentralWidget.pri)

To use **QGoodWindow** in your project add `#include <QGoodWindow>` in your code, then use  `QGoodWindow`  as  `QMainWindow`, for example, create an instance of  `QGoodWindow`  or sub class it.

To use **QGoodCentralWidget**  add `#include <QGoodCentralWidget>` in your code, set it as central widget of **QGoodWindow** and set to the **QGoodCentralWidget** it's own central widget.

**API:**

To see the full list of public APIs see [the docs](docs).

*Note:*

To get the required build packages on Ubuntu based OS, use:

	sudo apt install build-essential libgl1-mesa-dev libgtk2.0-dev libxkbcommon-x11-dev

**Credits:**

- The dark theme present on GoodShowCase example and the example itself is based on this awesome work: https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle, and the light theme was inspired on this dark theme.
- The GoodShowCaseGL example is based on the OpenGL textures example provided by [Qt](https://doc.qt.io/qt-5/qtopengl-textures-example.html).

**Contributing:**

You can contribute with suggestions, writing issues on [Issues](https://github.com/antonypro/QGoodWindow/issues) or filling pull requests.

To see what has changed between versions, please see the [CHANGELOG](CHANGELOG.md).
