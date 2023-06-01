#include "syscall.h"

int socketFD;

#define PORT 2816

void makeRequest(int option);

int mainMenuHandle();

int handleFigure();

int handleServerRequest();

int ConnectServer();

void processRequest(int socketId, int option, char* animal);

int main() {
    int option = 1;

    while (option != 0) {
        switch (option)
        {
        case 1:
            makeRequest(option);
            option = mainMenuHandle();
            break;
        case 2:
            makeRequest(option);
            option = handleFigure();
            break;
        case 3:
            option = handleServerRequest();
            break;
        default:
            break;
        }
    }
}

void makeRequest(int option) {

}

int mainMenuHandle() {

}

int handleFigure() {
    char animal[20];
}

int handleServerRequest() {

}

int ConnectServer() {

}


// processRequest syscall
// params: socket id, option, char* animal


// inputRequest syscall