cmake_minimum_required(VERSION 3.5)
project(Harbor_CLS)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

option(ASAN_BUILD "Enable AddressSanitizer for the project" OFF)
if (ASAN_BUILD)
    message(STATUS "CMAKE: AddressSanitizer enabled.")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

option(TSAN_BUILD "CMAKE: Enable ThreadSanitizer for the project" OFF)
if (TSAN_BUILD)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
endif()

add_compile_options(-Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror)

add_subdirectory(HarborCLS_Library)
add_subdirectory(Servers/LegoFigureMakerCommon)
add_subdirectory(Servers/IntermediaryServer)
add_subdirectory(Servers/PiecesServer)
#[[add_subdirectory(HarborCLS_Tests)
add_subdirectory(googletest)]]
add_subdirectory(HarborCLS_Library/Common/DependenciesManagement/Hypodermic)
