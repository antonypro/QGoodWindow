#The MIT License (MIT)

#Copyright © 2018-2024 Antonio Dias (https://github.com/antonypro)

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

win32{
goodmakedir1.commands += $(CHK_DIR_EXISTS) $$shell_path($$INSTALL_DIR) $(MKDIR) $$shell_path($$INSTALL_DIR)
goodmakedir2.commands += $(CHK_DIR_EXISTS) $$shell_path($$HEADER_DESTINATION) $(MKDIR) $$shell_path($$HEADER_DESTINATION)
goodmakedir3.commands += $(CHK_DIR_EXISTS) $$shell_path($$HEADER_DESTINATION/src) $(MKDIR) $$shell_path($$HEADER_DESTINATION/src)
} else {
goodmakedir1.commands += $(MKDIR) $$shell_path($$INSTALL_DIR)
goodmakedir2.commands += $(MKDIR) $$shell_path($$HEADER_DESTINATION)
goodmakedir3.commands += $(MKDIR) $$shell_path($$HEADER_DESTINATION/src)
}

first.depends += $(first) \
goodmakedir1 \
goodmakedir2 \
goodmakedir3

export(first.depends)
export(goodmakedir1.commands)
export(goodmakedir2.commands)
export(goodmakedir3.commands)

QMAKE_EXTRA_TARGETS += first \
goodmakedir1 \
goodmakedir2 \
goodmakedir3
