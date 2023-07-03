
#include "Generics/Socket/Socket.hpp"
#include <iostream>

#include "PiecesServer.hpp"

int main () {
    PiecesServer piecesServer("PiecesServer/legoFile.txt");
    piecesServer.readLegoSourceFile("PiecesServer/legoFile.txt");

    piecesServer.start();
  
}