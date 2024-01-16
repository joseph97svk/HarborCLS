cmake_minimum_required(VERSION 3.27)
project(IntermediaryServer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE PROJECT_SOURCES
        "*.cpp"
        "*.hpp")

add_executable(IntermediaryServer ${PROJECT_SOURCES})
target_link_libraries(IntermediaryServer HarborCLS_Library LegoFigureMakerCommon)
