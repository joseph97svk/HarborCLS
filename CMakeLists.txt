cmake_minimum_required(VERSION 3.27)
project(lego_figure_maker)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-fmodules-ts)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(COMMON_DIR HarborCLS)
file(GLOB_RECURSE COMMON_SOURCES
        "${COMMON_DIR}/*.cpp"
        "${COMMON_DIR}/*.hpp")

add_library(HarborCLS ${COMMON_SOURCES})
target_include_directories(HarborCLS PUBLIC ${COMMON_DIR})

target_link_libraries(HarborCLS OpenSSL::SSL OpenSSL::Crypto Threads::Threads)

#add_subdirectory(OldServers/IntermediaryServer)
#add_subdirectory(OldServers/PiecesServer)
add_subdirectory(Servers/IntermediaryServer)