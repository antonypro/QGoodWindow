#The MIT License (MIT)

#Copyright © 2023 Antonio Dias (https://github.com/antonypro)

#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:

#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

copygoodwidgetheader1.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodCentralWidget/QGoodCentralWidget) $$shell_path($$HEADER_DESTINATION)
copygoodwidgetheader2.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodCentralWidget/src/qgoodcentralwidget.h) $$shell_path($$HEADER_DESTINATION/src)

copygoodwidgetheader3.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodCentralWidget/src/qgoodcentralwidget_helper.h) $$shell_path($$HEADER_DESTINATION/src)

plugin {
copygoodwidgetheader4.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodCentralWidget/lib-helper/qgoodcentralwidget_global.h) $$shell_path($$HEADER_DESTINATION/src)
} else {
copygoodwidgetheader4.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodCentralWidget/src/qgoodcentralwidget_global.h) $$shell_path($$HEADER_DESTINATION/src)
}

first.depends += \
copygoodwidgetheader1 \
copygoodwidgetheader2 \
copygoodwidgetheader3 \
copygoodwidgetheader4

export(copygoodwidgetheader1.commands)
export(copygoodwidgetheader2.commands)
export(copygoodwidgetheader3.commands)
export(copygoodwidgetheader4.commands)

QMAKE_EXTRA_TARGETS += \
copygoodwidgetheader1 \
copygoodwidgetheader2 \
copygoodwidgetheader3 \
copygoodwidgetheader4
