**QGoodWindow** - border less window implementation for Windows.

**White example:**  
![QGoodWindow White!](https://raw.githubusercontent.com/antonypro/QGoodWindow/master/image/white_example.png)

**Black example:**  
![QGoodWindow Black!](https://raw.githubusercontent.com/antonypro/QGoodWindow/master/image/black_example.png)

**QGoodWindow** is an implementation of the same visual concept introduced by Microsoft on Visual Studio 2012, Office 2013 and respective newer versions for Qt 5 on Windows, where the app has full control of the whole window, including the title bar, client area and borders. All are handled as client area. This allows whole window customization, including addition of Qt Widgets on the title bar, like notification label, and input widgets like line edit and push button.

This project has also a compatibility layer for other operating systems where these levels of customization aren’t available, like Linux, so you can keep most of your code compatible with these systems, except code for the border and title bar customization.

**QGoodWindow** Usage:

To use  **QGoodWindow**  in your project add  `include(path_to/QGoodWindowFiles/QGoodWindowFiles.pri)`  in your  `.pro`  file,  `#include <QGoodWindow>`  in your code, then use  `QGoodWindow`  as  `QMainWindow`, for example, create an instance of  `QGoodWindow`  or sub class it. To see this project in action, see the examples that comes with it!

**QGoodWindow** features:

- All the low level WinAPI call are done behind the scenes, and you can use the Qt APIs for, for example, maximize, minimize, hide/show window, toggle full screen mode and handle window close.
- The minimize animation introduced with Windows Aero on Windows Vista, and the maximize animation introduced on Windows 10 works.
- **QGoodWindow** can handle changes between Windows Aero, Windows Basic Mode and Classic themes without visual changes on the window itself, as if no change was done.
- You can create `QDialog` and children's(`QMessageBox` for example) setting **QGoodWindow** as parent and the child will be placed in the center of your window and **QGoodWindow** will be blocked and re-enabled as original `QMainWindow`.
- **QGoodWindow** on normal state (not maximized and not full screen) keeps a 1 pixel border around the window for resizing, and you can fully customize the color of the border by just changing it in your widget, but since 1 pixel border is too low width for allow resizing, **QGoodWindow** also includes a secondary window that acts both as a shadow and extension for resize border, all with the Windows Snap features preserved.
- You can change the margins of the title bar at any moment as needed, with allows put a QWidget in the title bar  that needs mouse/keyboard interaction by just increasing right margin and return to the original right margin when the widget being removed from the title bar. More details below.

**QGoodWindow** margins:

You can change the margins used in the title bar with this function:

    void QGoodWindow::setMargins(int title_bar_height, int icon_width, int left, int right)
    
  Where:
  
- `title_bar_height` is the height of the title bar, you need to draw your window, this functions only tells to the application where the move area of window is, and in full screen mode it should be set to 0. For example: `setMargins(0, 0, 0, 0);`
And you need to hide your title bar on this mode, and show it when window state is restored, and call `setMargins` with the appropriate values back.
- `icon_width` is useful when your window shows an icon like the red icon on the example, set it to the actual width of your icon area and it's height will be the same as `title_bar_height`, or set to 0 if not showing an icon or don't want show window context menu clicking on this area or not close application on double clicking on it, for example.
- `left` is the width of the area in the title bar that needs mouse/keyboard focus, or in other words an area that needs to belong to your application and not to move/maximize the window itself, on the left of the title bar. The `left` margin is added at the right of `icon_width`.
- `right` is the same as `left` but on right of the window, this can include yours custom minimize, maximize/restore, close and maybe additional buttons if you are drawing some.

See the image below to get a visual representation of the title bar margins:
![QGoodWindow Margins!](https://raw.githubusercontent.com/antonypro/QGoodWindow/master/image/title_bar_margins.png)
