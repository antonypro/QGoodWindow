**QGoodWindow** - border less window implementation for Qt 5 and Qt 6 on Windows, Linux and macOS.

**QGoodWindow** in dark mode on Windows 11:
![](https://raw.githubusercontent.com/antonypro/QGoodWindow/master/image/qgoodwindow-dark.png)

**QGoodWindow** in light mode on Windows 11:
![](https://raw.githubusercontent.com/antonypro/QGoodWindow/master/image/qgoodwindow-light.png)

**QGoodWindow** is an implementation of the same visual concept introduced by Microsoft on Visual Studio 2012, Office 2013 and respective newer versions, for Qt 5 and Qt 6 on Windows, Linux and macOS, where the app has full control of the whole window, including the title bar, client area and borders. All are handled as client area. This allows whole window customization, including addition of Qt Widgets on the title bar.

This project has also a compatibility layer for other operating systems where these levels of customization aren't available, like Android, so you can keep most of your code compatible with these systems, except code for the border and title bar customization.

**QGoodWindow** Usage:

To use  **QGoodWindow**  in your project add  `include(path_to/QGoodWindow/QGoodWindow.pri)`  in your  `.pro`  file,  `#include <QGoodWindow>`  in your code, then use  `QGoodWindow`  as  `QMainWindow`, for example, create an instance of  `QGoodWindow`  or sub class it. To see this project in action, see the examples that comes with this project!

*Linux:*

To get the required build packages on Debian based OS, use:

	sudo apt install build-essential libgl1-mesa-dev libgtk2.0-dev libxkbcommon-x11-dev

**QGoodWindow** features and characteristics for each supported OS:

*Windows, Linux and macOS:*

- **QGoodWindow** can handle changes between Light and Dark themes and inform the change trough `void systemThemeChanged()` `SIGNAL` and you can verify if theme is dark with static API  `bool isSystemThemeDark()`.

- **QGoodWindow** informs if the current OS window manager must draw the window borders or if this task should be done by you with the static API `bool shouldBordersBeDrawnBySystem()`.

*Windows:*

- All the low level WinAPI call are done behind the scenes, and you can use the Qt APIs for, for example, maximize, minimize, hide/show window, toggle full screen mode and handle window close.
- The minimize animation introduced with Windows Aero on Windows Vista, and the maximize animation introduced on Windows 10 works.
- The Windows 11 rounded corners and Snap window on maximize button works.
- **QGoodWindow** can handle changes between Windows Aero, Windows Basic Mode and Classic themes without visual changes on the window itself, as if no change was done.
- You can create `QDialog` and children's(`QMessageBox` for example) setting **QGoodWindow** as parent and the child will be placed in the center of your window and **QGoodWindow** will be blocked and re-enabled as original `QMainWindow`.
- **QGoodWindow** on normal state (not maximized and not full screen) keeps a 1 pixel border around the window for resizing, (Not on Windows 11 where borders and shadow are native, in any OS use the API `bool shouldBordersBeDrawnBySystem()` to know if system handle borders drawing.) and you can fully customize the color of the border by just changing it in your widget, but since 1 pixel border is a small width for allow resizing, **QGoodWindow** also includes a secondary window that acts both as a shadow and extension for resize border, all with the Windows Snap features preserved.
- On Windows, you can pass a clear color to class constructor, which allows set a color for the window while it is opening but its contents is not fully loaded.

*Linux:*

 - On Linux, X11 and Wayland trough XWayland are supported.
 - A frameless `QMainWindow` is created and resize borders are added around the window. The shadow is provided by the Window Manager.
 - Different from Windows, there is *no* 1 pixel resize border around the window, but you can keep a line around the widget to keep the visual identity of the window if you want.

*macOS:*

 - On macOS, the resize, the border and the shadow are provided by the OS, only the move of the window is handled internally.
 - The native caption buttons are removed, and you can put your own buttons.

On any supported OS, you can verify if **QGoodWindow** is enabled by checking if `QGOODWINDOW` is defined on your code and on qmake see if `qgoodwindow` `CONFIG` is set.

**QGoodWindow** caption buttons:

- To provide the maximize/restore button Snap window on Windows 11 when mouse hovers this button. You must handle the caption buttons calling:

      void setCaptionButtonsHandled(bool handled, const Qt::Corner &corner)

	while corner is the corner where the captions buttons stays, and they always should be in the bounds of left or right margin explained below. Then set the regions of the minimize maximize and close buttons relative to left or right margin bounds with these APIs:

      void setMinimizeMask(const QRegion &mask)
      void setMaximizeMask(const QRegion &mask)
      void setCloseMask(const QRegion &mask)

	This can handle the captions buttons not only on Windows 11, but on any Windows, Linux or macOS OS.
	
	To handle the caption buttons state changes, listen for the `SIGNAL`: 

      void captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state)

**QGoodWindow** margins:

- You can change the margins of the title bar at any moment as needed, which allows put a QWidget in the title bar that needs mouse/keyboard interaction by just increasing left/right margin or change the left/right mask and return to the original margin or mask when the widget is removed from the title bar. More details below.

You can change the margins used in the title bar with this function:

    void QGoodWindow::setMargins(int title_bar_height, int icon_width, int left, int right)
    
  Where:
  

- `title_bar_height` is the height of the title bar, you need to draw your window, this functions only tells to the application where the move area of window is, and in full screen mode it should be set to 0. For example: `setMargins(0, 0, 0, 0)`
And you need to hide your title bar on this mode, and show it when window state is restored, and call `setMargins` with the appropriate values back.
- `icon_width` is useful when your window shows an icon like the red icon on the example, set it to the actual width of your icon area and it's height will be the same as `title_bar_height`, or set to 0 if not showing an icon or don't want show window context menu clicking on this area or not close application on double clicking on it, for example. (Note: on Linux and macOS the click and double click will be handled equally as on title bar, no menu or close happens on these OSes.)
- `left` is the width of the area in the title bar that needs mouse/keyboard focus, or in other words an area that needs to belong to your application and not to move/maximize the window itself, it is on the left of the title bar. The `left` margin is added at the right of `icon_width`.
- `right` is the same as `left` but on right of the window, this can include yours custom minimize, maximize/restore, close and maybe additional buttons if you are drawing some.

You can add a mask for the left and/or right margin, which enables custom areas for mouse hit test, passing a `QRegion` or a `QBitmap` to the functions:

    void setLeftMask(const QRegion &mask)
    void setRightMask(const QRegion &mask)

where the pixels with value `Qt::color1` belongs to the application and `Qt::color0` belongs to OS window manager.

The size of the mask should follow the size gave by the following functions:

    QSize leftMaskSize()
    QSize rightMaskSize()

**Note:** Anytime you call `QGoodWindow::setMargins()` the masks will be reset, so after each call to `QGoodWindow::setMargins()` you must call the set mask functions if you have custom masks.

See the image below to get a visual representation of the title bar margins:

![](https://raw.githubusercontent.com/antonypro/QGoodWindow/master/image/margins.png)

Or see the zoomed version:

![](https://raw.githubusercontent.com/antonypro/QGoodWindow/master/image/margins-left.png)

![](https://raw.githubusercontent.com/antonypro/QGoodWindow/master/image/margins-right.png)

**API:**

To see the full list of public APIs see the [API](https://github.com/antonypro/QGoodWindow/blob/master/API.md)

**Credits:**

- The dark theme present on GoodShowCase example is based on this awesome work: https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle, and the light theme was inspired on this dark theme.
- The GoodShowCaseGL example is based on the OpenGL textures example provided by [Qt](https://doc.qt.io/qt-5/qtopengl-textures-example.html).

**Contributing:**

You can contribute with suggestions, issues report on [Issues](https://github.com/antonypro/QGoodWindow/issues) or sending pull requests.

To see what has changed between versions, please see the [CHANGELOG](https://github.com/antonypro/QGoodWindow/blob/master/CHANGELOG.md).
