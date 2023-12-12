#The MIT License (MIT)

#Copyright © 2022-2023 Antonio Dias (https://github.com/antonypro)

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

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/qgoodwindow.cpp ${CMAKE_CURRENT_LIST_DIR}/src/qgoodwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/src/qgoodstateholder.cpp ${CMAKE_CURRENT_LIST_DIR}/src/qgoodstateholder.h
    ${CMAKE_CURRENT_LIST_DIR}/src/lightstyle.cpp ${CMAKE_CURRENT_LIST_DIR}/src/lightstyle.h
    ${CMAKE_CURRENT_LIST_DIR}/src/darkstyle.cpp ${CMAKE_CURRENT_LIST_DIR}/src/darkstyle.h
    ${CMAKE_CURRENT_LIST_DIR}/src/stylecommon.cpp ${CMAKE_CURRENT_LIST_DIR}/src/stylecommon.h
    ${CMAKE_CURRENT_LIST_DIR}/src/intcommon.h
 
    ${CMAKE_CURRENT_LIST_DIR}/src/qgoodwindow_style.qrc
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    UNICODE
)

if(WIN32 AND ${QT_VERSION_MAJOR} EQUAL 5)
    find_package(Qt5 REQUIRED
        WinExtras
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt5::WinExtras
    )
endif()

if(${QT_VERSION_MAJOR} EQUAL 5)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        QT_VERSION_QT5
    )
endif()

if(${QT_VERSION_MAJOR} EQUAL 6)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        QT_VERSION_QT6
    )
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        Gdi32
        User32
    )

    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD true)
endif()

if(NOT no_qgoodwindow)
    if(WIN32) #Windows
        set(qgoodwindow TRUE)

        target_sources(${PROJECT_NAME} PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src/common.h
            ${CMAKE_CURRENT_LIST_DIR}/src/qgooddialog.cpp ${CMAKE_CURRENT_LIST_DIR}/src/qgooddialog.h
            ${CMAKE_CURRENT_LIST_DIR}/src/shadow.cpp ${CMAKE_CURRENT_LIST_DIR}/src/shadow.h
        )

        target_compile_definitions(${PROJECT_NAME} PUBLIC
            QGOODWINDOW
        )

        find_package(Qt${QT_VERSION_MAJOR} REQUIRED
            Core
            Gui
            Widgets
        )

        target_link_libraries(${PROJECT_NAME} PUBLIC
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Gui
            Qt${QT_VERSION_MAJOR}::Widgets
        )
    endif() #Windows

    if(UNIX AND NOT APPLE AND NOT ANDROID) #Linux
        set(qgoodwindow TRUE)

        target_sources(${PROJECT_NAME} PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src/qgooddialog.cpp ${CMAKE_CURRENT_LIST_DIR}/src/qgooddialog.h
            ${CMAKE_CURRENT_LIST_DIR}/src/shadow.cpp ${CMAKE_CURRENT_LIST_DIR}/src/shadow.h
        )

        target_compile_definitions(${PROJECT_NAME} PUBLIC
            QGOODWINDOW
        )

        target_compile_options(${PROJECT_NAME} PUBLIC
            -Wno-deprecated-declarations
        )

        if(${QT_VERSION_MAJOR} EQUAL 5)
            find_package(PkgConfig REQUIRED)
            pkg_check_modules(GTK2 REQUIRED "gtk+-2.0")
            if(DEFINED GTK2_INCLUDE_DIRS)
                target_include_directories(${PROJECT_NAME} PUBLIC ${GTK2_INCLUDE_DIRS})
            endif()
            if(DEFINED GTK2_LIBRARY_DIRS)
                target_link_directories(${PROJECT_NAME} PUBLIC ${GTK2_LIBRARY_DIRS})
            endif()
            if(DEFINED GTK2_CFLAGS_OTHER)
                target_compile_options(${PROJECT_NAME} PUBLIC ${GTK2_CFLAGS_OTHER})
            endif()
            if(DEFINED GTK2_LIBRARIES)
                target_link_libraries(${PROJECT_NAME} PUBLIC ${GTK2_LIBRARIES})
            endif()
        endif()

        if(${QT_VERSION_MAJOR} EQUAL 6)
            find_package(PkgConfig REQUIRED)
            pkg_check_modules(GTK3 REQUIRED "gtk+-3.0")
            if(DEFINED GTK3_INCLUDE_DIRS)
                target_include_directories(${PROJECT_NAME} PUBLIC ${GTK3_INCLUDE_DIRS})
            endif()
            if(DEFINED GTK3_LIBRARY_DIRS)
                target_link_directories(${PROJECT_NAME} PUBLIC ${GTK3_LIBRARY_DIRS})
            endif()
            if(DEFINED GTK3_CFLAGS_OTHER)
                target_compile_options(${PROJECT_NAME} PUBLIC ${GTK3_CFLAGS_OTHER})
            endif()
            if(DEFINED GTK3_LIBRARIES)
                target_link_libraries(${PROJECT_NAME} PUBLIC ${GTK3_LIBRARIES})
            endif()
        endif()

        target_link_libraries(${PROJECT_NAME} PUBLIC
            X11
        )

        find_package(Qt${QT_VERSION_MAJOR} REQUIRED
            Core
            Gui
            Widgets
            Test
        )

        target_link_libraries(${PROJECT_NAME} PUBLIC
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Gui
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Test
        )

        if(${QT_VERSION_MAJOR} EQUAL 5)
            find_package(Qt${QT_VERSION_MAJOR} REQUIRED
                X11Extras
            )

            target_link_libraries(${PROJECT_NAME} PUBLIC
                Qt5::X11Extras
            )
        endif()

        if(${QT_VERSION_MAJOR} EQUAL 6)
            target_include_directories(${PROJECT_NAME} PUBLIC
                ${Qt6Gui_PRIVATE_INCLUDE_DIRS}
            )
        endif()
    endif() #Linux

    if(APPLE) #macOS
        set(qgoodwindow TRUE)

        target_sources(${PROJECT_NAME} PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src/macosnative.h ${CMAKE_CURRENT_LIST_DIR}/src/macosnative.mm
            ${CMAKE_CURRENT_LIST_DIR}/src/notification.cpp ${CMAKE_CURRENT_LIST_DIR}/src/notification.h
            ${CMAKE_CURRENT_LIST_DIR}/src/qgooddialog.cpp ${CMAKE_CURRENT_LIST_DIR}/src/qgooddialog.h
        )

        target_compile_definitions(${PROJECT_NAME} PUBLIC
            QGOODWINDOW
        )

        find_package(Qt${QT_VERSION_MAJOR} REQUIRED
            Core
            Gui
            Widgets
        )

        target_link_libraries(${PROJECT_NAME} PUBLIC
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Gui
            Qt${QT_VERSION_MAJOR}::Widgets
        )

        target_link_libraries(${PROJECT_NAME} PUBLIC
            "-framework AppKit"
            "-framework Cocoa"
            "-framework Foundation"
        )
    endif() #macOS
endif() #NOT no_qgoodwindow
