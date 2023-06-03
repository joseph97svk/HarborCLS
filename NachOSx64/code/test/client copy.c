#include "syscall.h"

int socketFD;

int bufferFile;

#define PORT 2816

// G0 Esteban
// G1 Johana
// G2 Joseph

void makeRequest(int option); // G1

int mainMenuHandle(); // G1

int handleFigure(); // G2

int handleServerRequest(); // G2

int ConnectServer(); // G1

//void processRequest(int socketId, int option, char* animal); // G0

int readControlledInput(int min, int max, char exception) {

} // G0

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

void processRequest(int socketId, int option, char* animal, int figurePos); // G0

int handleFigure() { // Joseph
    Write("0.Close\n1.Main Menu\n2.Assemble\nr.Refresh\n", 41, 1);

    int choice = readControlledInput(0, 2, 'r');
    if (choice == 0) {
        return 0;
    }

    if (choice == 'r') {
        return 2;
    }

    if (choice == 2) {
        return 3;
    }

    return 1;
}

int handleServerRequest() { // Joseph
    // connect to new socket
    int serverSocket = Socket(AF_INET_NachOS, SOCK_STREAM_NachOS, 1);
    Connect(serverSocket, "awa", PORT);

    char buffer[120];
    int pos = 0;
    int size = 0;
    // send pieces
    while (buffer[0] != 4) {
        size = Read(buffer, -1, bufferFile);
        if (pos % 2 == 0) {
            Write(buffer, size, socketFD);
        } else {
            Write();
        }
        
        pos++;
    }


    // receive response
    char buffer[2];
    buffer[1] = 0;

    Read(buffer, 2, serverSocket);

    // generate answer
    if (buffer[0] == '1') {
        Write("Assembled\n", 10, 1);
    } else {
        Write("Not assembled\n", 14, 1);
    }
}

int ConnectServer() {

}


// processRequest syscall
// params: socket id, option, char* animal


// inputRequest syscall