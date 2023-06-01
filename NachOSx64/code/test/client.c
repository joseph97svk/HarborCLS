#include "syscall.h"

int socketFD;

#define PORT 2816

// G0 Esteban
// G1 Johana
// G2 Joseph

void makeRequest(int option); // G1

int mainMenuHandle(); // G1

int handleFigure(); // G2

int handleServerRequest(); // G2

int ConnectServer(); // G1

void processRequest(int socketId, int option, char* animal); // G0

void readControlledInput(int min, int max, char exception); // G0

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