## class `QGoodWindow` 

`
class QGoodWindow
  : public QMainWindow
`

**QGoodWindow** class contains the public API's to control the behavior of the customized window.

### Members

#### `public  explicit QGoodWindow(QWidget * parent, const QColor & clear_color)` 

Constructor of *QGoodWindow*.

On Windows creates the native window, turns the `QMainWindow` as a native widget, creates the shadow, initialize default values and calls the `QMainWindow` parent constructor.

On Linux creates the frame less `QMainWindow`, use the shadow only to create resize borders, and the real shadow is draw by current Linux window manager.

On macOS creates a `QMainWindow` with full access to the title bar, and hide native minimize, zoom and close buttons.

#### `public  ~QGoodWindow()` 

Destructor of *QGoodWindow*.

#### `public WId winId() const` 

Returns the window id of the *QGoodWindow*.

#### `public void setWindowFlags(Qt::WindowFlags flags)` 

*QGoodWindow* handles flags internally, but this function could be used to create a always on top window.

#### `public Qt::WindowFlags windowFlags() const` 

Returns the window flags of the *QGoodWindow*.

#### `{signal} public void captionButtonStateChanged(const QGoodWindow::CaptionButtonState & state)` 

On handled caption buttons, this SIGNAL report the state of these buttons.

#### `{signal} public void systemThemeChanged()` 

Notify that the system has changed between light and dark mode.

#### `{signal} public void captionButtonsVisibilityChangedOnMacOS()` 

Notify that the visibility of caption buttons have changed on macOS.

#### `{slot} public void setNativeDarkModeEnabledOnWindows(bool dark)` 

Set native dark mode on Windows to *dark*.

#### `{slot} public bool isNativeDarkModeEnabledOnWindows() const` 

Returns if native dark mode is enabled on Windows.

#### `{slot} public void setNativeBorderColorOnWindows(const QColor & color)` 

Set native border color on Windows to *color*.

#### `{slot} public void setNativeCaptionButtonsVisibleOnMac(bool visible)` 

Set native caption buttons on macOS visibility to *visible*.

#### `{slot} public bool isNativeCaptionButtonsVisibleOnMac() const` 

Returns if native caption buttons are visible on macOS.

#### `{slot} public QRect titleBarButtonsRectOnMacOS() const` 

Returns native caption buttons rect on macOS.

#### `{slot} public void setTitleBarHeight(int height)` 

Set title bar height for *QGoodWindow*.

#### `{slot} public void setIconWidth(int width)` 

Set current icon width on the left side of the title bar of *QGoodWindow*.

#### `{slot} public int titleBarHeight() const` 

On Windows, Linux and macOS, returns the actual title bar height, on other OSes returns 0.

#### `{slot} public int iconWidth() const` 

On Windows, Linux and macOS, return the actual icon width, on other OSes returns 0.

#### `{slot} public QRect titleBarRect() const` 

Rect that contains the whole title bar.

#### `{slot} public void setTitleBarMask(const QRegion & mask)` 

Set the mask for the customized title bar.

#### `{slot} public void setMinimizeMask(const QRegion & mask)` 

Set the location and shape of handled minimize button, relative to title bar rect.

#### `{slot} public void setMaximizeMask(const QRegion & mask)` 

Set the location and shape of handled maximize button, relative to title bar rect.

#### `{slot} public void setCloseMask(const QRegion & mask)` 

Set the location and shape of handled close button, relative to title bar rect.

#### `{slot} public QRegion titleBarMask() const` 

Get the mask for the customized title bar.

#### `{slot} public QRegion minimizeMask() const` 

Get the location and shape of handled minimize button, relative to title bar rect.

#### `{slot} public QRegion maximizeMask() const` 

Get the location and shape of handled maximize button, relative to title bar rect.

#### `{slot} public QRegion closeMask() const` 

Get the location and shape of handled close button, relative to title bar rect.

#### `{slot} public void setCentralWidget(QWidget * widget)` 

Set central widget of *QGoodWindow* to *widget*.

#### `{slot} public QWidget * centralWidget() const` 

Returns the central widget of *QGoodWindow*.

#### `{slot} public QSize sizeHint() const` 

Get the size hint of *QGoodWindow*.

#### `{slot} public QSize minimumSizeHint() const` 

Get the minimum size hint of *QGoodWindow*.

#### `{slot} public void setFixedSize(int w, int h)` 

Set fixed size for *QGoodWindow* to width *w* and height *h*.

#### `{slot} public void setFixedSize(const QSize & size)` 

Set fixed size for *QGoodWindow* to *size*.

#### `{slot} public void setMinimumSize(const QSize & size)` 

Set minimum size for *QGoodWindow* to *size*.

#### `{slot} public void setMaximumSize(const QSize & size)` 

Set maximum size for *QGoodWindow* to *size*.

#### `{slot} public void setMinimumWidth(int w)` 

Set minimum width for *QGoodWindow* to *w*.

#### `{slot} public void setMinimumHeight(int h)` 

Set minimum height for *QGoodWindow* to *h*.

#### `{slot} public void setMaximumWidth(int w)` 

Set maximum width for *QGoodWindow* to *w*.

#### `{slot} public void setMaximumHeight(int h)` 

Set maximum height for *QGoodWindow* to *h*.

#### `{slot} public QSize minimumSize() const` 

Returns the *QGoodWindow* minimum size.

#### `{slot} public QSize maximumSize() const` 

Returns the *QGoodWindow* maximum size.

#### `{slot} public int minimumWidth() const` 

Returns minimum width for *QGoodWindow*

#### `{slot} public int minimumHeight() const` 

Returns minimum height for *QGoodWindow*

#### `{slot} public int maximumWidth() const` 

Returns maximum width for *QGoodWindow*

#### `{slot} public int maximumHeight() const` 

Returns maximum height for *QGoodWindow*

#### `{slot} public QRect normalGeometry() const` 

Returns the geometry for *QGoodWindow* when it's state is no state.

#### `{slot} public QRect frameGeometry() const` 

Returns the geometry for *QGoodWindow* including extended frame and excluding shadow.

#### `{slot} public QRect geometry() const` 

Returns the client area geometry.

#### `{slot} public QRect rect() const` 

Returns the client area size, position is always `QPoint(0, 0)`.

#### `{slot} public QPoint pos() const` 

Position of the window on screen.

#### `{slot} public QSize size() const` 

Size of the window on screen.

#### `{slot} public int x() const` 

X position of the window on screen.

#### `{slot} public int y() const` 

Y position of the window on screen.

#### `{slot} public int width() const` 

Width of the window.

#### `{slot} public int height() const` 

Height of the window.

#### `{slot} public void move(int x, int y)` 

Move the window to *x* - *y* coordinates.

#### `{slot} public void move(const QPoint & pos)` 

Move the window to *pos*.

#### `{slot} public void resize(int width, int height)` 

Resize the window to *width* - *height* size.

#### `{slot} public void resize(const QSize & size)` 

Resize the window to *size*.

#### `{slot} public void setGeometry(int x, int y, int w, int h)` 

Set geometry to pos *x* - *y*, width *w* and height *h*.

#### `{slot} public void setGeometry(const QRect & rect)` 

Set geometry to *rect*.

#### `{slot} public void activateWindow()` 

Activates the *QGoodWindow*.

#### `{slot} public void show()` 

Shows the *QGoodWindow*.

#### `{slot} public void showNormal()` 

Shows or restore the *QGoodWindow*.

#### `{slot} public void showMaximized()` 

Shows or maximize the *QGoodWindow*.

#### `{slot} public void showMinimized()` 

Minimize the *QGoodWindow*.

#### `{slot} public void showFullScreen()` 

Turns the *QGoodWindow* into full screen mode. Including the title bar.

#### `{slot} public void hide()` 

Hide the *QGoodWindow*.

#### `{slot} public bool close()` 

Try to close the *QGoodWindow*, returns true if event is accepted or false otherwise.

#### `{slot} public bool isVisible() const` 

Returns if the *QGoodWindow* is visible or not.

#### `{slot} public bool isEnabled() const` 

Returns if the *QGoodWindow* is enabled or not.

#### `{slot} public bool isActiveWindow() const` 

Returns if the *QGoodWindow* is the foreground window or not.

#### `{slot} public bool isMaximized() const` 

Returns if the *QGoodWindow* is maximized or not.

#### `{slot} public bool isMinimized() const` 

Returns if the *QGoodWindow* is minimized or not.

#### `{slot} public bool isFullScreen() const` 

Returns if the *QGoodWindow* is in full screen mode or not.

#### `{slot} public Qt::WindowStates windowState() const` 

Returns the *QGoodWindow* state.

#### `{slot} public void setWindowState(Qt::WindowStates state)` 

Sets the state of the *QGoodWindow* to *state*.

#### `{slot} public QWindow * windowHandle() const` 

Returns the window handle of the *QGoodWindow*.

#### `{slot} public qreal windowOpacity() const` 

Returns the opacity of the *QGoodWindow*.

#### `{slot} public void setWindowOpacity(qreal level)` 

Sets the opacity of the *QGoodWindow* to *level*. Where 0.0 is fully transparent and 1.0 fully opaque.

#### `{slot} public QString windowTitle() const` 

Returns the title of the *QGoodWindow*.

#### `{slot} public void setWindowTitle(const QString & title)` 

Sets the title of the *QGoodWindow* to *title*.

#### `{slot} public QIcon windowIcon() const` 

Returns the icon of the *QGoodWindow*.

#### `{slot} public void setWindowIcon(const QIcon & icon)` 

Sets the icon of the *QGoodWindow* to *icon*.

#### `{slot} public QByteArray saveGeometry() const` 

Returns a copy of the *QGoodWindow* geometry to restore it later.

#### `{slot} public bool restoreGeometry(const QByteArray & geometry)` 

Restore *QGoodWindow* to previous geometry *geometry*.

#### `enum CaptionButtonState` 

 Values                         | Descriptions                                
--------------------------------|---------------------------------------------
MinimizeHoverEnter            | Minimize button hover enter.
MinimizeHoverLeave            | Minimize button hover leave.
MinimizePress            | Minimize button press.
MinimizeRelease            | Minimize button release.
MinimizeClicked            | Minimize button clicked.
MaximizeHoverEnter            | Maximize or restore button hover enter.
MaximizeHoverLeave            | Maximize or restore button hover leave.
MaximizePress            | Maximize or restore button press.
MaximizeRelease            | Maximize or restore button release.
MaximizeClicked            | Maximize or restore button clicked.
CloseHoverEnter            | Close button hover enter.
CloseHoverLeave            | Close button hover leave.
ClosePress            | Close button press.
CloseRelease            | Close button release.
CloseClicked            | Close button clicked.

Enum that contains caption buttons states when it's states are handled by *QGoodWindow*.

#### `public static QString version()` 

Returns the *QGoodWindow* version.

#### `public static void aboutQGoodWindow(QGoodWindow * parent, const QString & title)` 

Shows a message box about *QGoodWindow*, with it's version and license.

#### `public static void setup()` 

Call this function to setup *QApplication* for *QGoodWindow* usage.

#### `public static bool isSystemThemeDark()` 

Returns if the current system theme is dark or not.

#### `public static bool shouldBordersBeDrawnBySystem()` 

Returns true if system draw borders and false if your app should do it.

#### `public static int execDialog(QDialog * dialog, QGoodWindow * child_gw, QGoodWindow * parent_gw)` 

Show modal frame less *QDialog*, inside window *child_gw*, with parent *parent_gw*.

#### `public static void setAppDarkTheme()` 

Set the app theme to the dark theme.

#### `public static void setAppLightTheme()` 

Set the app theme to the light theme.

#### `public static QGoodStateHolder * qGoodStateHolderInstance()` 

Get the global state holder.

Generated by [Moxygen](https://github.com/sourcey/moxygen)