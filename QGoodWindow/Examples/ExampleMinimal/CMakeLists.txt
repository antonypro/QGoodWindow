#The MIT License (MIT)

#Copyright © 2022 Antonio Dias

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

cmake_minimum_required(VERSION 3.12)
project(ExampleMinimal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED)

# set(no_qgoodwindow TRUE) #This flag disable QGoodWindow

if(NOT DEFINED no_qgoodwindow)
    set(no_qgoodwindow OFF CACHE BOOL "This flag disable QGoodWindow")
endif()

add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp mainwindow.h
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../QGoodWindow/QGoodWindow.cmake)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED
    Core
    Gui
    Widgets
)

if(WIN32 AND ${QT_VERSION_MAJOR} EQUAL 5)
    find_package(Qt5 REQUIRED
        WinExtras
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::WinExtras
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
