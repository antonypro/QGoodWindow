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

cmake_minimum_required(VERSION 3.14)
project(QGoodWindow)
enable_language(CXX)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED)

if(${QT_VERSION_MAJOR} EQUAL 5)
    set(CMAKE_CXX_STANDARD 11)
endif()
if(${QT_VERSION_MAJOR} EQUAL 6)
    set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(shared_library TRUE) #Build shared library

if(NOT DEFINED shared_library)
    set(shared_library OFF CACHE BOOL "Build shared library")
endif()

#set(with_central_widget TRUE) #Build with QGoodCentralWidget

if(NOT DEFINED with_central_widget)
    set(with_central_widget OFF CACHE BOOL "Build with QGoodCentralWidget")
endif()

if(shared_library)
    add_library(${PROJECT_NAME} SHARED)
else()
    add_library(${PROJECT_NAME} STATIC)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_KEYWORDS)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Set build mode")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release MinSizeRel RelWithDebInfo Debug)

set(HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/src/qgoodwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/src/intcommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/src/qgoodstateholder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/src/lightstyle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/src/darkstyle.h
)

install(FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/QGoodWindow
        DESTINATION include
        COMPONENT Headers
)

install(FILES ${HEADER_FILES}
        DESTINATION include/src
        COMPONENT Headers
)

install(FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/lib-helper/qgoodwindow_helper.h
        DESTINATION include/src
        COMPONENT Headers
)

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        COMPONENT Libraries
)

if(shared_library)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        QGOODWINDOW_LIBRARY QGOODWINDOW_SHARED_LIBRARY
    )
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/lib-helper/qgoodwindow_global.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/src
        COMPONENT Headers
    )
else()
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/src/qgoodwindow_global.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/src
        COMPONENT Headers
    )
endif()

if(with_central_widget)
    set(HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodCentralWidget/src/qgoodcentralwidget.h
    )

    install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodCentralWidget/QGoodCentralWidget
            DESTINATION include
            COMPONENT Headers
    )

    install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodCentralWidget/lib-helper/qgoodcentralwidget_helper.h
            DESTINATION include/src
            COMPONENT Headers
    )

    if(shared_library)
        install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodCentralWidget/lib-helper/qgoodcentralwidget_global.h
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/src
            COMPONENT Headers
        )
    else()
        install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/../QGoodCentralWidget/src/qgoodcentralwidget_global.h
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/src
            COMPONENT Headers
        )
    endif()
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../QGoodWindow/QGoodWindow.cmake)

if(with_central_widget)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../QGoodCentralWidget/QGoodCentralWidget.cmake)
endif()
