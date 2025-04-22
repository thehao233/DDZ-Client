cmake_minimum_required(VERSION 3.5)

project(ddz-client VERSION 0.1 LANGUAGES CXX)

# 自动处理.ui文件，最后生成ui_xxx.h文件
set(CMAKE_AUTOUIC ON)
# 自动处理元对象相关的宏 Q_OBJECT
set(CMAKE_AUTOMOC ON)
# 自动处理qrc资源文件，会为其生成c++代码，最后链接到可执行文件中
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
# 要求cmake版本必须符合，否则不会进行编译
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 搜索Qt相关组件
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

include_directories(
    ${PROJECT_SOURCE_DIR}/card
    ${PROJECT_SOURCE_DIR}/windows
    ${PROJECT_SOURCE_DIR}/control
    ${PROJECT_SOURCE_DIR}/player
    ${PROJECT_SOURCE_DIR}/strategy
    ${PROJECT_SOURCE_DIR}/thread
    ${PROJECT_SOURCE_DIR}/tcp
    ${PROJECT_SOURCE_DIR}/serialize
    ${PROJECT_SOURCE_DIR}/crypto
    # 测试用例
    ${PROJECT_SOURCE_DIR}/test

    "D:\\software\\Win64OpenSSL\\OpenSSL-Win64\\include"
    "D:\\software\\Protobuf\\Protobuf\\include"
)

add_subdirectory(card)
add_subdirectory(windows)
add_subdirectory(control)
add_subdirectory(player)
add_subdirectory(strategy)
add_subdirectory(thread)
add_subdirectory(tcp)
add_subdirectory(serialize)
add_subdirectory(crypto)
# 测试用例
add_subdirectory(test)

set(PROJECT_SOURCES
        main.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ddz-client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ddz-client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ddz-client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ddz-client
            ${PROJECT_SOURCES}
            res.qrc
        )
    endif()
endif()

# 链接库文件
target_link_libraries(ddz-client PRIVATE
  Qt${QT_VERSION_MAJOR}::Widgets
  card windows control player strategy thread tcp serialize crypto
  test
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ddz-client)
endif()
set_target_properties(ddz-client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ddz-client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ddz-client)
endif()
