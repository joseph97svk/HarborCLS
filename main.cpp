#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Client.hpp"

/**
  * main method
  * The main function creates a Client object, sets the type and IPv6 properties
  * and connects to a server with the connectServer() function.
  *
  * @return 0 to indicate successful execution.
  *
 **/
int main() {
    Client socket('s', false);  // create a Client object with type 's' and IPv6 as false
    socket.connectServer(); // connect to the server
    //socket.makeRequest("menu", RequestType::MenuRequest);  // make a request for the menus
    socket.Close();  // close the connection

    return 0;  // indicate successful execution
}