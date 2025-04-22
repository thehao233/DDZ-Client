cmake_minimum_required(VERSION 3.5)
project(windows)

# 搜索源文件（.cpp .c等）
aux_source_directory(./ SRC)

# 搜索头文件和ui文件
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB UIS ${PROJECT_SOURCE_DIR}/*.ui)

add_library(windows STATIC ${SRC} ${HEADERS} ${UIS})

target_link_libraries(windows PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
)
