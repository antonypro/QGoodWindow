# Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[`QGoodWindow`](#class_q_good_window) | **[QGoodWindow](#class_q_good_window)** class contains the public API's to control the behavior of the customized window.

## class `QGoodWindow` 

```
class QGoodWindow
  : public QMainWindow
```

**[QGoodWindow](#class_q_good_window)** class contains the public API's to control the behavior of the customized window.

On Windows, **[QGoodWindow](#class_q_good_window)** class inherits from `QMainWindow` which is used as a native widget that creates, fill and control the native window of **[QGoodWindow](#class_q_good_window)**.

On Linux and macOS the **[QGoodWindow](#class_q_good_window)** behaves like a frameless `QMainWindow`.

### Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public QPointer< QTimer > `[`m_theme_change_timer`](#class_q_good_window_1ac4ca75f9edff5ee46e4228fd469378c8) | Reserved.
`public  explicit `[`QGoodWindow`](#class_q_good_window_1a916f53453692ca63d5a0a88fa45ee551)`(QWidget * parent,const QColor & clear_color)` | Constructor of *[QGoodWindow](#class_q_good_window)*.
`public  `[`~QGoodWindow`](#class_q_good_window_1aa27bed6af044ab6c8848e09b293bf652)`()` | Destructor of *[QGoodWindow](#class_q_good_window)*.
`public WId `[`winId`](#class_q_good_window_1a1b52ddd48685013a36e0fc63c307b31e)`() const` | Returns the window id of the *[QGoodWindow](#class_q_good_window)*.
`protected bool `[`event`](#class_q_good_window_1abd7ad1ef5460f05d814193246d33a8d5)`(QEvent * event)` | 
`protected bool `[`eventFilter`](#class_q_good_window_1a8ff4a3874874053566d7e4637a2ef93f)`(QObject * watched,QEvent * event)` | 
`protected bool `[`nativeEvent`](#class_q_good_window_1a616c8140a4e97b2850dcdef470b900e3)`(const QByteArray & eventType,void * message,qintptr * result)` | 
`{signal} public void `[`captionButtonStateChanged`](#class_q_good_window_1af3c066373584140887eafc6b271dbaf0)`(const `[`QGoodWindow::CaptionButtonState`](#class_q_good_window_1a4505dd0fef4c499b92af26b904d41ca5)` & state)` | On handled caption buttons, this SIGNAL report the state of these buttons.
`{signal} public void `[`systemThemeChanged`](#class_q_good_window_1a673d7fdb5489fefa1720128d1ae9cef1)`()` | Notify that the system has changed between light and dark mode.
`{slot} public static bool `[`isSystemThemeDark`](#class_q_good_window_1a735e8fdda7081dca6bf921792ec7971d)`()` | Returns if the current system theme is dark or not.
`{slot} public static bool `[`shouldBordersBeDrawnBySystem`](#class_q_good_window_1a2f645705c29bdf17d9cbf8eb782119c4)`()` | Returns if there is a one pixel margin around window for resizing or not, i.e. if system draw margins.
`{slot} public int `[`titleBarHeight`](#class_q_good_window_1ac99c3e1ec7b2df926b0678b35d77cf04)`() const` | On Windows, Linux and macOS, returns the actual title bar height, on other OSes returns 0.
`{slot} public int `[`iconWidth`](#class_q_good_window_1a72967117fd888b153e6935025e0b86f9)`() const` | On Windows, Linux and macOS, return the actual icon width, on other OSes returns 0.
`{slot} public int `[`leftMargin`](#class_q_good_window_1a2f9ff85dc2fb13c72c21e67d030ff605)`() const` | On Windows, Linux and macOS, returns the left margin of the customized title bar, on other OSes returns 0.
`{slot} public int `[`rightMargin`](#class_q_good_window_1a1bd0308536a0d5b7079ed1d7774b8603)`() const` | On Windows, Linux and macOS, returns the right margin of the customized title bar, on other OSes returns 0.
`{slot} public void `[`setMargins`](#class_q_good_window_1a67c66b86a563a1c405442f5310b168a6)`(int title_bar_height,int icon_width,int left,int right)` | Set the tile bar height, icon width, left and right margins of the customized title bar.
`{slot} public void `[`setLeftMask`](#class_q_good_window_1a8c67cdc47deb06e9bd8831b501621746)`(const QRegion & mask)` | Set the mask for the left margin of the customized title bar.
`{slot} public void `[`setRightMask`](#class_q_good_window_1a7e20133b12f58e37d31ad483f1d175c3)`(const QRegion & mask)` | Set the mask for the right margin of the customized title bar.
`{slot} public void `[`setCaptionButtonsHandled`](#class_q_good_window_1ae49b197d93440b024b813dba71b58e4a)`(bool handled,const Qt::Corner & corner)` | Set if the caption buttons should be handled by *[QGoodWindow](#class_q_good_window)* and on which *corner*, valid only top left and top right corners.
`{slot} public void `[`setMinimizeMask`](#class_q_good_window_1a3e43e92928199146195b3592ee34a8c7)`(const QRegion & mask)` | Set the location and shape of handled minimize button, relative to handled corner.
`{slot} public void `[`setMaximizeMask`](#class_q_good_window_1a0147567bd431b53c45d18d159bfa71f7)`(const QRegion & mask)` | Set the location and shape of handled maximize button, relative to handled corner.
`{slot} public void `[`setCloseMask`](#class_q_good_window_1aaca5721d64f6c8fddc39bae4464fa57a)`(const QRegion & mask)` | Set the location and shape of handled close button, relative to handled corner.
`{slot} public QSize `[`leftMaskSize`](#class_q_good_window_1af993e05f0f628eaa3803bc4d70f74167)`() const` | Get the size that should be the size of the mask on the left margin of the customized title bar.
`{slot} public QSize `[`rightMaskSize`](#class_q_good_window_1a2428749e8c0d2b514ffffded17c842b5)`() const` | Get the size that should be the size of the mask on the right margin of the customized title bar.
`{slot} public QRect `[`leftCaptionButtonsRect`](#class_q_good_window_1ad900c30d1e48e030ab71f3063d890431)`() const` | If caption buttons are handled on left corner, their buttons masks should be in the bounds of this rect.
`{slot} public QRect `[`rightCaptionButtonsRect`](#class_q_good_window_1aa35dbcc667414e439c549c1bd3d51efc)`() const` | If caption buttons are handled on right corner, their buttons masks should be in the bounds of this rect.
`{slot} public void `[`setFixedSize`](#class_q_good_window_1a53270bd72bef61fc910d1b2808402d50)`(int w,int h)` | Set fixed size for *[QGoodWindow](#class_q_good_window)* to width *w* and height *h*.
`{slot} public void `[`setFixedSize`](#class_q_good_window_1ab295d99af677da730d9be507d7b49edf)`(const QSize & size)` | Set fixed size for *[QGoodWindow](#class_q_good_window)* to *size*.
`{slot} public QRect `[`frameGeometry`](#class_q_good_window_1ac21802e061d764d08fe3aa28e2138884)`() const` | Returns the geometry for *[QGoodWindow](#class_q_good_window)* including extended frame and excluding shadow.
`{slot} public QRect `[`geometry`](#class_q_good_window_1a7c830f318aa8708e90cc90e6808e6b76)`() const` | Returns the client area geometry.
`{slot} public QRect `[`rect`](#class_q_good_window_1a6537a41cebd8c316d9340e048eba6ec0)`() const` | Returns the client area size, position is always `QPoint(0, 0)`.
`{slot} public QPoint `[`pos`](#class_q_good_window_1ac24ab76741bbebd65338e2d564b1f662)`() const` | Position of the window on screen.
`{slot} public QSize `[`size`](#class_q_good_window_1afeac66d7d10069a9aac5b43da07bdfba)`() const` | Size of the window on screen.
`{slot} public int `[`x`](#class_q_good_window_1a6c57af6bb9b150cb90dfae63e3c32a08)`() const` | X position of the window on screen.
`{slot} public int `[`y`](#class_q_good_window_1af054cc3d252a1de56402978fc6eecc68)`() const` | Y position of the window on screen.
`{slot} public int `[`width`](#class_q_good_window_1ad318d9e81353eec57f53a969dd1b5c3a)`() const` | Width of the window.
`{slot} public int `[`height`](#class_q_good_window_1a4b3083e616ab469b9ea746f0c8649ba8)`() const` | Height of the window.
`{slot} public void `[`move`](#class_q_good_window_1aac9bad6135777ced0c14806da45c3b18)`(int x,int y)` | Move the window to *x* - *y* coordinates.
`{slot} public void `[`move`](#class_q_good_window_1a3eda325347fc3ad650a0c530ac87ed5f)`(const QPoint & pos)` | Move the window to *pos*.
`{slot} public void `[`resize`](#class_q_good_window_1aed63b7bade8a1a3ab6cceb3c18155974)`(int width,int height)` | Resize the window to *width* - *height* size.
`{slot} public void `[`resize`](#class_q_good_window_1a20b73656397c01f9a796a1cda9bcf0af)`(const QSize & size)` | Resize the window to *size*.
`{slot} public void `[`setGeometry`](#class_q_good_window_1ab3a5ccd3b67ffbbdd42db5e9b21aecbf)`(int x,int y,int w,int h)` | Set geometry to pos *x* - *y*, width *w* and height *h*.
`{slot} public void `[`setGeometry`](#class_q_good_window_1a3f4b6457272c18f66701b3da25df4480)`(const QRect & rect)` | Set geometry to *rect*.
`{slot} public void `[`activateWindow`](#class_q_good_window_1a14ed83ff786dc0e5c18136636ecf43c2)`()` | Activates the *[QGoodWindow](#class_q_good_window)*.
`{slot} public void `[`show`](#class_q_good_window_1acce4ce202d6eedefb2c80491748a1cb8)`()` | Shows the *[QGoodWindow](#class_q_good_window)*.
`{slot} public void `[`showNormal`](#class_q_good_window_1a63d1056a421d79280531cd320425c253)`()` | Shows or restore the *[QGoodWindow](#class_q_good_window)*.
`{slot} public void `[`showMaximized`](#class_q_good_window_1a02281a3a6cf845f0e9fcfc7e083b3757)`()` | Shows or maximize the *[QGoodWindow](#class_q_good_window)*.
`{slot} public void `[`showMinimized`](#class_q_good_window_1a8c8fb1c22c66770750f53cacb4d4fdeb)`()` | Minimize the *[QGoodWindow](#class_q_good_window)*.
`{slot} public void `[`showFullScreen`](#class_q_good_window_1a407d2a35bdb0671fcf836351900e97e1)`()` | Turns the *[QGoodWindow](#class_q_good_window)* into full screen mode. Including the title bar.
`{slot} public void `[`hide`](#class_q_good_window_1ae2e60054897ccd5653f5be932053bf04)`()` | Hide the *[QGoodWindow](#class_q_good_window)*.
`{slot} public void `[`close`](#class_q_good_window_1a41604e8c4279f25ff24682cffdffe230)`()` | Close the *[QGoodWindow](#class_q_good_window)*.
`{slot} public bool `[`isVisible`](#class_q_good_window_1a1d599244ada55a97cdd5c0fc3e6ed891)`() const` | Returns if the *[QGoodWindow](#class_q_good_window)* is visible or not.
`{slot} public bool `[`isEnabled`](#class_q_good_window_1a9f53ebbfa54d3977d0135a68b6e0c7e8)`() const` | Returns if the *[QGoodWindow](#class_q_good_window)* is enabled or not.
`{slot} public bool `[`isActiveWindow`](#class_q_good_window_1ad56a75e3e9fb8d269bf763af3352b0c0)`() const` | Returns if the *[QGoodWindow](#class_q_good_window)* is the foreground window or not.
`{slot} public bool `[`isMaximized`](#class_q_good_window_1add626760ca65a5efd91368bc4d9a306b)`() const` | Returns if the *[QGoodWindow](#class_q_good_window)* is maximized or not.
`{slot} public bool `[`isMinimized`](#class_q_good_window_1ac62107e80260a48bfb2fd13821996a42)`() const` | Returns if the *[QGoodWindow](#class_q_good_window)* is minimized or not.
`{slot} public bool `[`isFullScreen`](#class_q_good_window_1a1366e38b36d8e59d3bb736d3c6fac00d)`() const` | Returns if the *[QGoodWindow](#class_q_good_window)* is in full screen mode or not.
`{slot} public Qt::WindowStates `[`windowState`](#class_q_good_window_1a9e31832134e766874510bb21305a1e4f)`() const` | Returns the *[QGoodWindow](#class_q_good_window)* state.
`{slot} public void `[`setWindowState`](#class_q_good_window_1ac2eef59173dc60dd935d77abf7e81e8e)`(Qt::WindowStates state)` | Sets the state of the *[QGoodWindow](#class_q_good_window)* to *state*.
`{slot} public QWindow * `[`windowHandle`](#class_q_good_window_1aedb712f4033dc71ff9243cda662e3ce7)`() const` | Returns the window handle of the *[QGoodWindow](#class_q_good_window)*.
`{slot} public qreal `[`windowOpacity`](#class_q_good_window_1a34e6069ea0f2465d191d0dd31bd044c6)`() const` | Returns the opacity of the *[QGoodWindow](#class_q_good_window)*.
`{slot} public void `[`setWindowOpacity`](#class_q_good_window_1a16e016a2d550e4fc4770b983f49e533d)`(qreal level)` | Sets the opacity of the *[QGoodWindow](#class_q_good_window)* to *level*. Where 0.0 is fully transparent and 1.0 fully opaque.
`{slot} public QString `[`windowTitle`](#class_q_good_window_1a3beebeabcd7f92e6c998a6104364f548)`() const` | Returns the title of the *[QGoodWindow](#class_q_good_window)*.
`{slot} public void `[`setWindowTitle`](#class_q_good_window_1a0a6f77eb4866f502409c299860433c2c)`(const QString & title)` | Sets the title of the *[QGoodWindow](#class_q_good_window)* to *title*.
`{slot} public QIcon `[`windowIcon`](#class_q_good_window_1ae80c9ecab57a0f8196ccd9b70fa97c31)`() const` | Returns the icon of the *[QGoodWindow](#class_q_good_window)*.
`{slot} public void `[`setWindowIcon`](#class_q_good_window_1ad0a6092904769d19fd641f1d5e74bc56)`(const QIcon & icon)` | Sets the icon of the *[QGoodWindow](#class_q_good_window)* to *icon*.
`enum `[`CaptionButtonState`](#class_q_good_window_1a4505dd0fef4c499b92af26b904d41ca5) | Enum that contains caption buttons states when it's states are handled by *[QGoodWindow](#class_q_good_window)*.

### Members

#### `public QPointer< QTimer > `[`m_theme_change_timer`](#class_q_good_window_1ac4ca75f9edff5ee46e4228fd469378c8) 

Reserved.

#### `public  explicit `[`QGoodWindow`](#class_q_good_window_1a916f53453692ca63d5a0a88fa45ee551)`(QWidget * parent,const QColor & clear_color)` 

Constructor of *[QGoodWindow](#class_q_good_window)*.

On Windows creates the native window, turns the `QMainWindow` as a native widget, creates the shadow, initialize default values and calls the `QMainWindow` parent constructor.

On Linux creates the frameless `QMainWindow`, use the shadow only to create resize borders, and the real shadow is draw by current Linux window manager.

On macOS creates a `QMainWindow` with full access to the title bar, and hide native minimize, zoom and close buttons.

#### `public  `[`~QGoodWindow`](#class_q_good_window_1aa27bed6af044ab6c8848e09b293bf652)`()` 

Destructor of *[QGoodWindow](#class_q_good_window)*.

#### `public WId `[`winId`](#class_q_good_window_1a1b52ddd48685013a36e0fc63c307b31e)`() const` 

Returns the window id of the *[QGoodWindow](#class_q_good_window)*.

#### `protected bool `[`event`](#class_q_good_window_1abd7ad1ef5460f05d814193246d33a8d5)`(QEvent * event)` 

#### `protected bool `[`eventFilter`](#class_q_good_window_1a8ff4a3874874053566d7e4637a2ef93f)`(QObject * watched,QEvent * event)` 

#### `protected bool `[`nativeEvent`](#class_q_good_window_1a616c8140a4e97b2850dcdef470b900e3)`(const QByteArray & eventType,void * message,qintptr * result)` 

#### `{signal} public void `[`captionButtonStateChanged`](#class_q_good_window_1af3c066373584140887eafc6b271dbaf0)`(const `[`QGoodWindow::CaptionButtonState`](#class_q_good_window_1a4505dd0fef4c499b92af26b904d41ca5)` & state)` 

On handled caption buttons, this SIGNAL report the state of these buttons.

#### `{signal} public void `[`systemThemeChanged`](#class_q_good_window_1a673d7fdb5489fefa1720128d1ae9cef1)`()` 

Notify that the system has changed between light and dark mode.

#### `{slot} public static bool `[`isSystemThemeDark`](#class_q_good_window_1a735e8fdda7081dca6bf921792ec7971d)`()` 

Returns if the current system theme is dark or not.

#### `{slot} public static bool `[`shouldBordersBeDrawnBySystem`](#class_q_good_window_1a2f645705c29bdf17d9cbf8eb782119c4)`()` 

Returns if there is a one pixel margin around window for resizing or not, i.e. if system draw margins.

#### `{slot} public int `[`titleBarHeight`](#class_q_good_window_1ac99c3e1ec7b2df926b0678b35d77cf04)`() const` 

On Windows, Linux and macOS, returns the actual title bar height, on other OSes returns 0.

#### `{slot} public int `[`iconWidth`](#class_q_good_window_1a72967117fd888b153e6935025e0b86f9)`() const` 

On Windows, Linux and macOS, return the actual icon width, on other OSes returns 0.

#### `{slot} public int `[`leftMargin`](#class_q_good_window_1a2f9ff85dc2fb13c72c21e67d030ff605)`() const` 

On Windows, Linux and macOS, returns the left margin of the customized title bar, on other OSes returns 0.

#### `{slot} public int `[`rightMargin`](#class_q_good_window_1a1bd0308536a0d5b7079ed1d7774b8603)`() const` 

On Windows, Linux and macOS, returns the right margin of the customized title bar, on other OSes returns 0.

#### `{slot} public void `[`setMargins`](#class_q_good_window_1a67c66b86a563a1c405442f5310b168a6)`(int title_bar_height,int icon_width,int left,int right)` 

Set the tile bar height, icon width, left and right margins of the customized title bar.

#### `{slot} public void `[`setLeftMask`](#class_q_good_window_1a8c67cdc47deb06e9bd8831b501621746)`(const QRegion & mask)` 

Set the mask for the left margin of the customized title bar.

#### `{slot} public void `[`setRightMask`](#class_q_good_window_1a7e20133b12f58e37d31ad483f1d175c3)`(const QRegion & mask)` 

Set the mask for the right margin of the customized title bar.

#### `{slot} public void `[`setCaptionButtonsHandled`](#class_q_good_window_1ae49b197d93440b024b813dba71b58e4a)`(bool handled,const Qt::Corner & corner)` 

Set if the caption buttons should be handled by *[QGoodWindow](#class_q_good_window)* and on which *corner*, valid only top left and top right corners.

#### `{slot} public void `[`setMinimizeMask`](#class_q_good_window_1a3e43e92928199146195b3592ee34a8c7)`(const QRegion & mask)` 

Set the location and shape of handled minimize button, relative to handled corner.

#### `{slot} public void `[`setMaximizeMask`](#class_q_good_window_1a0147567bd431b53c45d18d159bfa71f7)`(const QRegion & mask)` 

Set the location and shape of handled maximize button, relative to handled corner.

#### `{slot} public void `[`setCloseMask`](#class_q_good_window_1aaca5721d64f6c8fddc39bae4464fa57a)`(const QRegion & mask)` 

Set the location and shape of handled close button, relative to handled corner.

#### `{slot} public QSize `[`leftMaskSize`](#class_q_good_window_1af993e05f0f628eaa3803bc4d70f74167)`() const` 

Get the size that should be the size of the mask on the left margin of the customized title bar.

#### `{slot} public QSize `[`rightMaskSize`](#class_q_good_window_1a2428749e8c0d2b514ffffded17c842b5)`() const` 

Get the size that should be the size of the mask on the right margin of the customized title bar.

#### `{slot} public QRect `[`leftCaptionButtonsRect`](#class_q_good_window_1ad900c30d1e48e030ab71f3063d890431)`() const` 

If caption buttons are handled on left corner, their buttons masks should be in the bounds of this rect.

#### `{slot} public QRect `[`rightCaptionButtonsRect`](#class_q_good_window_1aa35dbcc667414e439c549c1bd3d51efc)`() const` 

If caption buttons are handled on right corner, their buttons masks should be in the bounds of this rect.

#### `{slot} public void `[`setFixedSize`](#class_q_good_window_1a53270bd72bef61fc910d1b2808402d50)`(int w,int h)` 

Set fixed size for *[QGoodWindow](#class_q_good_window)* to width *w* and height *h*.

#### `{slot} public void `[`setFixedSize`](#class_q_good_window_1ab295d99af677da730d9be507d7b49edf)`(const QSize & size)` 

Set fixed size for *[QGoodWindow](#class_q_good_window)* to *size*.

#### `{slot} public QRect `[`frameGeometry`](#class_q_good_window_1ac21802e061d764d08fe3aa28e2138884)`() const` 

Returns the geometry for *[QGoodWindow](#class_q_good_window)* including extended frame and excluding shadow.

#### `{slot} public QRect `[`geometry`](#class_q_good_window_1a7c830f318aa8708e90cc90e6808e6b76)`() const` 

Returns the client area geometry.

#### `{slot} public QRect `[`rect`](#class_q_good_window_1a6537a41cebd8c316d9340e048eba6ec0)`() const` 

Returns the client area size, position is always `QPoint(0, 0)`.

#### `{slot} public QPoint `[`pos`](#class_q_good_window_1ac24ab76741bbebd65338e2d564b1f662)`() const` 

Position of the window on screen.

#### `{slot} public QSize `[`size`](#class_q_good_window_1afeac66d7d10069a9aac5b43da07bdfba)`() const` 

Size of the window on screen.

#### `{slot} public int `[`x`](#class_q_good_window_1a6c57af6bb9b150cb90dfae63e3c32a08)`() const` 

X position of the window on screen.

#### `{slot} public int `[`y`](#class_q_good_window_1af054cc3d252a1de56402978fc6eecc68)`() const` 

Y position of the window on screen.

#### `{slot} public int `[`width`](#class_q_good_window_1ad318d9e81353eec57f53a969dd1b5c3a)`() const` 

Width of the window.

#### `{slot} public int `[`height`](#class_q_good_window_1a4b3083e616ab469b9ea746f0c8649ba8)`() const` 

Height of the window.

#### `{slot} public void `[`move`](#class_q_good_window_1aac9bad6135777ced0c14806da45c3b18)`(int x,int y)` 

Move the window to *x* - *y* coordinates.

#### `{slot} public void `[`move`](#class_q_good_window_1a3eda325347fc3ad650a0c530ac87ed5f)`(const QPoint & pos)` 

Move the window to *pos*.

#### `{slot} public void `[`resize`](#class_q_good_window_1aed63b7bade8a1a3ab6cceb3c18155974)`(int width,int height)` 

Resize the window to *width* - *height* size.

#### `{slot} public void `[`resize`](#class_q_good_window_1a20b73656397c01f9a796a1cda9bcf0af)`(const QSize & size)` 

Resize the window to *size*.

#### `{slot} public void `[`setGeometry`](#class_q_good_window_1ab3a5ccd3b67ffbbdd42db5e9b21aecbf)`(int x,int y,int w,int h)` 

Set geometry to pos *x* - *y*, width *w* and height *h*.

#### `{slot} public void `[`setGeometry`](#class_q_good_window_1a3f4b6457272c18f66701b3da25df4480)`(const QRect & rect)` 

Set geometry to *rect*.

#### `{slot} public void `[`activateWindow`](#class_q_good_window_1a14ed83ff786dc0e5c18136636ecf43c2)`()` 

Activates the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public void `[`show`](#class_q_good_window_1acce4ce202d6eedefb2c80491748a1cb8)`()` 

Shows the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public void `[`showNormal`](#class_q_good_window_1a63d1056a421d79280531cd320425c253)`()` 

Shows or restore the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public void `[`showMaximized`](#class_q_good_window_1a02281a3a6cf845f0e9fcfc7e083b3757)`()` 

Shows or maximize the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public void `[`showMinimized`](#class_q_good_window_1a8c8fb1c22c66770750f53cacb4d4fdeb)`()` 

Minimize the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public void `[`showFullScreen`](#class_q_good_window_1a407d2a35bdb0671fcf836351900e97e1)`()` 

Turns the *[QGoodWindow](#class_q_good_window)* into full screen mode. Including the title bar.

#### `{slot} public void `[`hide`](#class_q_good_window_1ae2e60054897ccd5653f5be932053bf04)`()` 

Hide the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public void `[`close`](#class_q_good_window_1a41604e8c4279f25ff24682cffdffe230)`()` 

Close the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public bool `[`isVisible`](#class_q_good_window_1a1d599244ada55a97cdd5c0fc3e6ed891)`() const` 

Returns if the *[QGoodWindow](#class_q_good_window)* is visible or not.

#### `{slot} public bool `[`isEnabled`](#class_q_good_window_1a9f53ebbfa54d3977d0135a68b6e0c7e8)`() const` 

Returns if the *[QGoodWindow](#class_q_good_window)* is enabled or not.

#### `{slot} public bool `[`isActiveWindow`](#class_q_good_window_1ad56a75e3e9fb8d269bf763af3352b0c0)`() const` 

Returns if the *[QGoodWindow](#class_q_good_window)* is the foreground window or not.

#### `{slot} public bool `[`isMaximized`](#class_q_good_window_1add626760ca65a5efd91368bc4d9a306b)`() const` 

Returns if the *[QGoodWindow](#class_q_good_window)* is maximized or not.

#### `{slot} public bool `[`isMinimized`](#class_q_good_window_1ac62107e80260a48bfb2fd13821996a42)`() const` 

Returns if the *[QGoodWindow](#class_q_good_window)* is minimized or not.

#### `{slot} public bool `[`isFullScreen`](#class_q_good_window_1a1366e38b36d8e59d3bb736d3c6fac00d)`() const` 

Returns if the *[QGoodWindow](#class_q_good_window)* is in full screen mode or not.

#### `{slot} public Qt::WindowStates `[`windowState`](#class_q_good_window_1a9e31832134e766874510bb21305a1e4f)`() const` 

Returns the *[QGoodWindow](#class_q_good_window)* state.

#### `{slot} public void `[`setWindowState`](#class_q_good_window_1ac2eef59173dc60dd935d77abf7e81e8e)`(Qt::WindowStates state)` 

Sets the state of the *[QGoodWindow](#class_q_good_window)* to *state*.

#### `{slot} public QWindow * `[`windowHandle`](#class_q_good_window_1aedb712f4033dc71ff9243cda662e3ce7)`() const` 

Returns the window handle of the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public qreal `[`windowOpacity`](#class_q_good_window_1a34e6069ea0f2465d191d0dd31bd044c6)`() const` 

Returns the opacity of the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public void `[`setWindowOpacity`](#class_q_good_window_1a16e016a2d550e4fc4770b983f49e533d)`(qreal level)` 

Sets the opacity of the *[QGoodWindow](#class_q_good_window)* to *level*. Where 0.0 is fully transparent and 1.0 fully opaque.

#### `{slot} public QString `[`windowTitle`](#class_q_good_window_1a3beebeabcd7f92e6c998a6104364f548)`() const` 

Returns the title of the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public void `[`setWindowTitle`](#class_q_good_window_1a0a6f77eb4866f502409c299860433c2c)`(const QString & title)` 

Sets the title of the *[QGoodWindow](#class_q_good_window)* to *title*.

#### `{slot} public QIcon `[`windowIcon`](#class_q_good_window_1ae80c9ecab57a0f8196ccd9b70fa97c31)`() const` 

Returns the icon of the *[QGoodWindow](#class_q_good_window)*.

#### `{slot} public void `[`setWindowIcon`](#class_q_good_window_1ad0a6092904769d19fd641f1d5e74bc56)`(const QIcon & icon)` 

Sets the icon of the *[QGoodWindow](#class_q_good_window)* to *icon*.

#### `enum `[`CaptionButtonState`](#class_q_good_window_1a4505dd0fef4c499b92af26b904d41ca5) 

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

Enum that contains caption buttons states when it's states are handled by *[QGoodWindow](#class_q_good_window)*.

Generated by [Moxygen](https://github.com/sourcey/moxygen)

