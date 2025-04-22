cmake_minimum_required(VERSION 3.5)
project(tcp)

# 搜索源文件和头文件
aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(tcp STATIC ${SRC} ${HEADERS})

# 公开头文件目录，使其他模块能找到 tcp.h
# target_include_directories(tcp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tcp PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    ws2_32
)
