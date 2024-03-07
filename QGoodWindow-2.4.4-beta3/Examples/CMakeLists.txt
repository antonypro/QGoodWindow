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

cmake_minimum_required(VERSION 3.14)
project(Examples)

#set(no_qgoodwindow TRUE) #This flag disable QGoodWindow

if(NOT DEFINED no_qgoodwindow)
    set(no_qgoodwindow OFF CACHE BOOL "This flag disable QGoodWindow")
endif()

add_subdirectory(ExampleMinimal)
add_subdirectory(ExampleMinimalWindow)
add_subdirectory(GoodPlayground)
add_subdirectory(GoodShowCase)
add_subdirectory(GoodShowCaseGL)
add_subdirectory(GoodShowCaseQML)
