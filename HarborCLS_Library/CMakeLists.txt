cmake_minimum_required(VERSION 3.27)
project(HarborCLS_Library)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-fmodules-ts)

file(GLOB_RECURSE COMMON_SOURCES
        "*.cpp"
        "*.hpp")

add_library(HarborCLS_Library ${COMMON_SOURCES})
target_include_directories(HarborCLS_Library PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(HarborCLS_Library OpenSSL::SSL OpenSSL::Crypto Threads::Threads)
