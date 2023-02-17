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

DESTDIR = $$shell_path($$INSTALL_DIR/lib)

HEADER_DESTINATION = $$INSTALL_DIR/include

win32{
mkdir1.commands += $(CHK_DIR_EXISTS) $$shell_path($$INSTALL_DIR) $(MKDIR) $$shell_path($$INSTALL_DIR)
mkdir2.commands += $(CHK_DIR_EXISTS) $$shell_path($$HEADER_DESTINATION) $(MKDIR) $$shell_path($$HEADER_DESTINATION)
mkdir3.commands += $(CHK_DIR_EXISTS) $$shell_path($$HEADER_DESTINATION/src) $(MKDIR) $$shell_path($$HEADER_DESTINATION/src)
} else {
mkdir1.commands += $(MKDIR) $$shell_path($$INSTALL_DIR)
mkdir2.commands += $(MKDIR) $$shell_path($$HEADER_DESTINATION)
mkdir3.commands += $(MKDIR) $$shell_path($$HEADER_DESTINATION/src)
}

copyheader1.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/QGoodWindow) $$shell_path($$HEADER_DESTINATION)
copyheader2.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodCentralWidget/QGoodCentralWidget) $$shell_path($$HEADER_DESTINATION)

copyheader3.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/qgoodwindow.h) $$shell_path($$HEADER_DESTINATION/src)
copyheader4.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/qgoodstateholder.h) $$shell_path($$HEADER_DESTINATION/src)
copyheader5.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/lightstyle.h) $$shell_path($$HEADER_DESTINATION/src)
copyheader6.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/src/darkstyle.h) $$shell_path($$HEADER_DESTINATION/src)
copyheader7.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodCentralWidget/src/qgoodcentralwidget.h) $$shell_path($$HEADER_DESTINATION/src)

copyheader8.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/lib-helper/qgoodwindow_helper.h) $$shell_path($$HEADER_DESTINATION/src)
copyheader9.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodCentralWidget/lib-helper/qgoodcentralwidget_helper.h) $$shell_path($$HEADER_DESTINATION/src)

copyheader10.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodWindow/lib-helper/qgoodwindow_global.h) $$shell_path($$HEADER_DESTINATION/src)
copyheader11.commands += $$QMAKE_COPY \
$$shell_path($$PWD/../../QGoodCentralWidget/lib-helper/qgoodcentralwidget_global.h) $$shell_path($$HEADER_DESTINATION/src)

first.depends = $(first) \
mkdir1 \
mkdir2 \
mkdir3 \
copyheader1 \
copyheader2 \
copyheader3 \
copyheader4 \
copyheader5 \
copyheader6 \
copyheader7 \
copyheader8 \
copyheader9 \
copyheader10 \
copyheader11

export(first.depends)
export(mkdir1.commands)
export(mkdir2.commands)
export(mkdir3.commands)
export(copyheader1.commands)
export(copyheader2.commands)
export(copyheader3.commands)
export(copyheader4.commands)
export(copyheader6.commands)
export(copyheader7.commands)
export(copyheader8.commands)
export(copyheader9.commands)
export(copyheader10.commands)
export(copyheader11.commands)

QMAKE_EXTRA_TARGETS += first \
mkdir1 \
mkdir2 \
mkdir3 \
copyheader1 \
copyheader2 \
copyheader3 \
copyheader4 \
copyheader5 \
copyheader6 \
copyheader7 \
copyheader8 \
copyheader9 \
copyheader10 \
copyheader11
