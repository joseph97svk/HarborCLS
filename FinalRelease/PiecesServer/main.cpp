
#include "Generics/Socket/Socket.hpp"
#include <iostream>
#include <csignal>
#include "PiecesServer.hpp"

/**
 * Signal handler function to handle different signals received by the program.
 * @param signal The signal received by the program.
 */
void signal_handler(int signal)
{
    if (signal == SIGTERM    // kill
        || signal == SIGINT  // ctrl + C
        || signal == SIGSTOP // ctrl + Z
    )
    {
        std::cout << "\nSTOP PROGRAM" << std::endl;
        PiecesServer::getInstance().stopServer();
    }
}

int main(int argc, char** argv)
{
    // Set signal handlers for SIGINT, SIGTERM and SIGSTOP signals
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    // signal(SIGSTOP, signal_handler);
    PiecesServer::getInstance().readLegoSourceFile("PiecesServer/legoFile.txt");

    PiecesServer::getInstance().start();
}