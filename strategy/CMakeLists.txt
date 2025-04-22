cmake_minimum_required(VERSION 3.5)
project(strategy)

# 搜索源文件和头文件
aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(strategy STATIC ${SRC} ${HEADERS})

target_link_libraries(strategy PRIVATE Qt${QT_VERSION_MAJOR}::Core)
