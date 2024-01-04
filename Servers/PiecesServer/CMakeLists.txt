cmake_minimum_required(VERSION 3.27)
project(PiecesServer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE PROJECT_SOURCES "*.cpp" "*.hpp")

add_executable(PiecesServer ${PROJECT_SOURCES})
target_link_libraries(PiecesServer HarborCLS_Library LegoFigureMakerCommon)
