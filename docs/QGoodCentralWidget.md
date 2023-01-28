## class `QGoodCentralWidget` 

`
class QGoodCentralWidget
  : public QWidget
`

**QGoodCentralWidget** class contains the public API's to control the behavior of **QGoodWindow**.

### Members

#### `public  explicit QGoodCentralWidget(QGoodWindow * gw)` 

Constructor of *QGoodCentralWidget*, is mandatory pass a valid instance of the parent *QGoodWindow*.

#### `public  ~QGoodCentralWidget()` 

Destructor of *QGoodCentralWidget*.

#### `{slot} public void setUnifiedTitleBarAndCentralWidget(bool unified)` 

Set the title bar and the central widget unified.

#### `{slot} public void setTitleBarMask(const QRegion & mask)` 

Set the title bar mask, the title bar widgets masks united with this mask.

#### `{slot} public QWidget * setLeftTitleBarWidget(QWidget * widget, bool transparent_for_mouse)` 

Set the left title bar widget and returns the previous widget or nullptr if none, delete this widget as needed. If the widget is transparent for mouse, but not it's children's set *transparent_for_mouse* to true.

#### `{slot} public QWidget * setRightTitleBarWidget(QWidget * widget, bool transparent_for_mouse)` 

Set the right title bar widget and returns the previous widget or nullptr if none, delete this widget as needed. If the widget is transparent for mouse, but not it's children's set *transparent_for_mouse* to true.

#### `{slot} public QWidget * setCenterTitleBarWidget(QWidget * widget, bool transparent_for_mouse)` 

Set the center title bar widget and returns the previous widget or nullptr if none, delete this widget as needed. If the widget is transparent for mouse, but not it's children's set *transparent_for_mouse* to true.

#### `{slot} public void setCentralWidget(QWidget * widget)` 

Set the central widget of *QGoodCentralWidget*.

#### `{slot} public void setTitleAlignment(const Qt::Alignment & alignment)` 

Set the alignment of *QGoodCentralWidget* title on the title bar. Note: If align to center and also set a central title bar widget the title will be aligned to the left.

#### `{slot} public void setTitleBarColor(const QColor & color)` 

Set the color of *QGoodCentralWidget* title bar.

#### `{slot} public void setActiveBorderColor(const QColor & color)` 

Set the color of *QGoodCentralWidget* border.

#### `{slot} public void setTitleBarVisible(bool visible)` 

Change the visibility of *QGoodCentralWidget* title bar.

#### `{slot} public void setTitleVisible(bool visible)` 

Change the visibility of *QGoodCentralWidget* title bar title.

#### `{slot} public void setIconVisible(bool visible)` 

Change the visibility of *QGoodCentralWidget* title bar icon.

#### `{slot} public void setTitleBarHeight(int height)` 

Change the title bar height to *height* multiplied to current pixel ratio.

#### `{slot} public void setCaptionButtonWidth(int width)` 

Change the caption button width to *width* multiplied to current pixel ratio.

#### `{slot} public bool isUnifiedTitleBarAndCentralWidget() const` 

Returns if the title bar and the central widget are unified.

#### `{slot} public QRect titleBarRect() const` 

Returns the title bar bounding rect.

#### `{slot} public QWidget * leftTitleBarWidget() const` 

Returns the left *QGoodCentralWidget* title bar widget or nullptr if none is set.

#### `{slot} public QWidget * rightTitleBarWidget() const` 

Returns the right *QGoodCentralWidget* title bar widget or nullptr if none is set.

#### `{slot} public QWidget * centerTitleBarWidget() const` 

Returns the center *QGoodCentralWidget* title bar widget or nullptr if none is set.

#### `{slot} public QWidget * centralWidget() const` 

Returns the *QGoodCentralWidget* central widget or nullptr if none is set.

#### `{slot} public Qt::Alignment titleAlignment() const` 

Returns the alignment of *QGoodCentralWidget* title on the title bar.

#### `{slot} public QColor titleBarColor() const` 

Returns the *QGoodCentralWidget* title bar color.

#### `{slot} public QColor activeBorderColor() const` 

Returns the *QGoodCentralWidget* border color.

#### `{slot} public bool isTitleBarVisible() const` 

Returns if the *QGoodCentralWidget* title bar is visible or not.

#### `{slot} public bool isTitleVisible() const` 

Returns if the *QGoodCentralWidget* title bar title is visible or not.

#### `{slot} public bool isIconVisible() const` 

Returns if the *QGoodCentralWidget* title bar icon is visible or not.

#### `{slot} public int titleBarHeight() const` 

Returns the *QGoodCentralWidget* title bar height.

#### `{slot} public int captionButtonWidth() const` 

Returns the *QGoodCentralWidget* title bar caption button width.

#### `{slot} public void updateWindow()` 

Update the *QGoodCentralWidget* state, it's called internally and must be called if changing *QGoodCentralWidget* metrics like hide or show a title bar widget.

#### `public static int execDialogWithWindow(QDialog * dialog, QGoodWindow * parent_gw, QGoodCentralWidget * base_gcw, QWidget * left_title_bar_widget, QWidget * right_title_bar_widget, bool title_visible, bool icon_visible)` 

Utility for showing a modal *QDialog* with customized title bar and borders. Pass the *QDialog* and the parent *QGoodWindow* and optionally a *QGoodCentralWidget* for mimic it's colors on the new window. Optionally pass a left title bar widget, a right title bar widget and set the visibility of title and icon on the new window.

Generated by [Moxygen](https://github.com/sourcey/moxygen)