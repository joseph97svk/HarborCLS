cmake_minimum_required(VERSION 3.27)
project(LegoFigureMakerCommon)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE PROJECT_SOURCES "*.cpp" "*.hpp")

add_library(LegoFigureMakerCommon SHARED ${PROJECT_SOURCES})
target_link_libraries(LegoFigureMakerCommon HarborCLS_Library)
