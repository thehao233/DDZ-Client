cmake_minimum_required(VERSION 3.5)
project(crypto)

# include_directories("D:\\software\\Win64OpenSSL\\OpenSSL-Win64\\include")

# 搜索源文件和头文件
aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(crypto STATIC ${SRC} ${HEADERS})

target_link_libraries(crypto PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    "D:\\software\\Win64OpenSSL\\OpenSSL-Win64\\lib\\libcrypto.lib"
)
