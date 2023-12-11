cmake_minimum_required(VERSION 3.27)
project(lego_figure_maker)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(COMMON_DIR HttpServer)
file(GLOB_RECURSE COMMON_SOURCES "${COMMON_DIR}/*.cpp" "${COMMON_DIR}/*.hpp")

add_library(HttpServer ${COMMON_SOURCES})
target_include_directories(HttpServer PUBLIC ${COMMON_DIR})

target_link_libraries(HttpServer OpenSSL::SSL OpenSSL::Crypto Threads::Threads)

add_subdirectory(Servers/IntermediaryServer)
add_subdirectory(Servers/PiecesServer)

#[[include_directories(HttpServer/Client)
include_directories(HttpServer/IntermediaryServer)
include_directories(HttpServer/IntermediaryServer/Generics)
include_directories(HttpServer/IntermediaryServer/Generics/Socket)
include_directories(HttpServer/IntermediaryServer/Handlers)
include_directories(HttpServer/PiecesServer)
include_directories(HttpServer/PiecesServer/Generics)
include_directories(HttpServer/PiecesServer/Generics/Socket)

add_executable(lego_figure_maker
        HttpServer/IntermediaryServer/Generics/Socket/Socket.cpp
        HttpServer/IntermediaryServer/Generics/Socket/Socket.hpp
        HttpServer/IntermediaryServer/Generics/common.hpp
        HttpServer/IntermediaryServer/Generics/Handler.hpp
        HttpServer/IntermediaryServer/Generics/Listener.hpp
        HttpServer/IntermediaryServer/Generics/ProtocolHeader.hpp
        HttpServer/IntermediaryServer/Generics/Queue.hpp
        HttpServer/IntermediaryServer/Generics/RoutingMap.hpp
        HttpServer/IntermediaryServer/Generics/Thread.hpp
        HttpServer/IntermediaryServer/Handlers/ClientHandler.hpp
        HttpServer/IntermediaryServer/Handlers/RequestHandler.hpp
        HttpServer/IntermediaryServer/Handlers/ResponseHandler.hpp
        HttpServer/IntermediaryServer/Handlers/UDPHandler.hpp
        HttpServer/IntermediaryServer/chickiIcon.ico
        HttpServer/IntermediaryServer/chickiIcon.png
        HttpServer/IntermediaryServer/esjojo.pem
        HttpServer/IntermediaryServer/IntermediaryServer.hpp
        HttpServer/IntermediaryServer/key.pem
        HttpServer/IntermediaryServer/Listeners.hpp
        HttpServer/IntermediaryServer/main.cpp
        HttpServer/IntermediaryServer/makefile
        HttpServer/PiecesServer/Generics/Socket/Socket.cpp
        HttpServer/PiecesServer/Generics/Socket/Socket.hpp
        HttpServer/PiecesServer/Generics/Handler.hpp
        HttpServer/PiecesServer/Generics/Listener.hpp
        HttpServer/PiecesServer/Generics/ProtocolHeader.hpp
        HttpServer/PiecesServer/Generics/Queue.hpp
        HttpServer/PiecesServer/Generics/Thread.hpp
        HttpServer/PiecesServer/lego/pictures/brick-1x1-eyes-dark-grey.jpg
        HttpServer/PiecesServer/lego/pictures/brick-1x1-piel.jpg
        HttpServer/PiecesServer/lego/pictures/brick-1x2-slope-inverted-red.jpg
        HttpServer/PiecesServer/lego/pictures/brick-1x4-blanco.jpg
        HttpServer/PiecesServer/lego/pictures/brick-1x4-piel.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x1-blanco.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x2-grey.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x2-piel.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x2-slope-gray.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x2-slope-inverted-grey.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x2-slope-inverted-red.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x2-slope-red.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x3-blanco.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x3-piel.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x3-red.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x4-blanco.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x4-grey.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x4-piel.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x4-plate-gray.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x4-red.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x4-rojo.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x4-yellow.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x6-plate-grey.jpg
        HttpServer/PiecesServer/lego/pictures/brick-2x8-red.jpg
        HttpServer/PiecesServer/lego/pictures/flag-yellow.jpg
        HttpServer/PiecesServer/lego/Chicki.jpeg
        HttpServer/PiecesServer/lego/Chicki.jpg
        HttpServer/PiecesServer/lego/Dragon.jpg
        HttpServer/PiecesServer/ci0123.pem
        HttpServer/PiecesServer/common.hpp
        HttpServer/PiecesServer/esjojo.pem
        HttpServer/PiecesServer/Handlers.hpp
        HttpServer/PiecesServer/key.pem
        HttpServer/PiecesServer/kk.txt
        HttpServer/PiecesServer/legoFile.txt
        HttpServer/PiecesServer/Listeners.hpp
        HttpServer/PiecesServer/main.cpp
        HttpServer/PiecesServer/makefile
        HttpServer/PiecesServer/new.jpg
        HttpServer/PiecesServer/newAwa.jpg
        HttpServer/PiecesServer/PiecesServer.hpp
        HttpServer/PiecesServer/PiecesServerStructure.hpp
        Servers/IntermediaryServer/Handlers/ClientHandler.hpp)]]

