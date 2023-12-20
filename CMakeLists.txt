cmake_minimum_required(VERSION 3.27)
project(Harbor_CLS)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-fmodules-ts)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(COMMON_DIR HarborCLS_Library )
file(GLOB_RECURSE COMMON_SOURCES
        "${COMMON_DIR}/*.cpp"
        "${COMMON_DIR}/*.hpp")

add_library(HarborCLS_Library ${COMMON_SOURCES})
target_include_directories(HarborCLS_Library PUBLIC ${COMMON_DIR})

target_link_libraries(HarborCLS_Library OpenSSL::SSL OpenSSL::Crypto Threads::Threads)

#add_subdirectory(OldServers/IntermediaryServer)
#add_subdirectory(OldServers/PiecesServer)
add_subdirectory(Servers/IntermediaryServer)