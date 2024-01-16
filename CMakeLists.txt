cmake_minimum_required(VERSION 3.27)
project(Harbor_CLS)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(HarborCLS_Library)
add_subdirectory(Servers/LegoFigureMakerCommon)
add_subdirectory(Servers/IntermediaryServer)
add_subdirectory(Servers/PiecesServer)
#[[add_subdirectory(HarborCLS_Tests)
add_subdirectory(googletest)]]
add_subdirectory(HarborCLS_Library/Common/DependenciesManagement/Hypodermic)
