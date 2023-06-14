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

copygoodheader1.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/QGoodWindow) $$shell_path($$HEADER_DESTINATION)

copygoodheader2.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/qgoodwindow.h) $$shell_path($$HEADER_DESTINATION/src)
copygoodheader3.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/intcommon.h) $$shell_path($$HEADER_DESTINATION/src)
copygoodheader4.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/qgoodstateholder.h) $$shell_path($$HEADER_DESTINATION/src)
copygoodheader5.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/lightstyle.h) $$shell_path($$HEADER_DESTINATION/src)
copygoodheader6.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/darkstyle.h) $$shell_path($$HEADER_DESTINATION/src)

copygoodheader7.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/lib-helper/qgoodwindow_helper.h) $$shell_path($$HEADER_DESTINATION/src)

plugin {
copygoodheader8.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/lib-helper/qgoodwindow_global.h) $$shell_path($$HEADER_DESTINATION/src)
} else {
copygoodheader8.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/qgoodwindow_global.h) $$shell_path($$HEADER_DESTINATION/src)
}

first.depends += \
copygoodheader1 \
copygoodheader2 \
copygoodheader3 \
copygoodheader4 \
copygoodheader5 \
copygoodheader6 \
copygoodheader7 \
copygoodheader8

export(copygoodheader1.commands)
export(copygoodheader2.commands)
export(copygoodheader3.commands)
export(copygoodheader4.commands)
export(copygoodheader6.commands)
export(copygoodheader7.commands)
export(copygoodheader8.commands)

QMAKE_EXTRA_TARGETS += \
copygoodheader1 \
copygoodheader2 \
copygoodheader3 \
copygoodheader4 \
copygoodheader5 \
copygoodheader6 \
copygoodheader7 \
copygoodheader8
