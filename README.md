# QGoodWindow - border less window for Qt 5

![QGoodWindow Demo!](https://raw.githubusercontent.com/antonypro/QGoodWindow/master/image/demo.png)

**QGoodWindow** is a border less window for Microsoft Windows OS, it's easy to implement and maintain across other OSes like Android and Linux desktops, where the border less window are unavailable.

**QGoodWindow** inherits from `QMainWindow`, then just set the central widget likely you do in `QMainWindow` class and it's sub classes, however, when showing a modal dialog, you must pass `QWidget *parentForModal();` function as a parent instead the `QGoodWindow` instance itself, also you need to draw the title bar yourself, as a part of `centralWidget`, on other OSes than Windows the native title bar is used! If you're working on a cross platform project you need to differentiate Windows, where you draw the title bar, and others OSes where you don't!

## Usage:
To use **QGoodWindow** in your project add `include(path_to/QGoodWindowFiles/QGoodWindowFiles.pri)` in your `.pro` file, `#include <QGoodWindow>` in your code, then use `QGoodWindow` as `QMainWindow`, for example, create an instance of `QGoodWindow` or sub class it. To see this project in action, see the demos that comes with it!
