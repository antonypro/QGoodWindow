/*
The MIT License (MIT)

Copyright © 2022-2023 Antonio Dias (https://github.com/antonypro)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef QGOODSTATEHOLDER
#define QGOODSTATEHOLDER

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "qgoodwindow_global.h"

//\cond HIDDEN_SYMBOLS
class SHAREDLIBSHARED_EXPORT QGoodStateHolder : public QObject
{
    Q_OBJECT
public:
    static QGoodStateHolder *instance()
    {
        static QGoodStateHolder instance;
        return &instance;
    }

private:
    explicit QGoodStateHolder() : QObject()
    {
        m_dark = false;
    }

    ~QGoodStateHolder()
    {

    }

signals:
    void currentThemeChanged();

public slots:
    bool isCurrentThemeDark() const
    {
        return m_dark;
    }

    void setCurrentThemeDark(bool dark)
    {
        m_dark = dark;

        emit currentThemeChanged();
    }

private:
    bool m_dark;
};
//\endcond

#define qGoodStateHolder QGoodStateHolder::instance()

#endif // QGOODSTATEHOLDER
