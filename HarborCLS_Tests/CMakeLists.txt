cmake_minimum_required(VERSION 3.27)
project(HarborCLS_Tests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCES "*.cpp")

link_libraries(OpenSSL::SSL OpenSSL::Crypto Threads::Threads)

set(DEFAULT_TARGET ${PROJECT_NAME})
add_executable(${DEFAULT_TARGET} ${PROJECT_SOURCES})

target_link_libraries(${DEFAULT_TARGET} HarborCLS_Library)
target_link_libraries(${DEFAULT_TARGET} gtest_main gtest gmock_main gmock)
target_include_directories(${DEFAULT_TARGET} PUBLIC "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
