cmake_minimum_required(VERSION 3.27)
project(IntermediaryServer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE PROJECT_SOURCES "*.cpp")

link_libraries(OpenSSL::SSL OpenSSL::Crypto Threads::Threads)

set(DEFAULT_TARGET ${PROJECT_NAME})
add_executable(${DEFAULT_TARGET} ${PROJECT_SOURCES})

target_link_libraries(${DEFAULT_TARGET} HarborCLS)