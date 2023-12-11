#include "syscall.h"

#define false 0
#define true 1

#define bool char

int socketFD;
int bufferId;
char* figureName;
int figureSize;
bool connected;

#define PORT 2816

#define serverHandleBufferSize 120

void makeRequest(int option);

int mainMenuHandle();

int handleFigure();

int handleServerRequest();

int ConnectServer();

int main() {
    char buffer[20];
    figureName = &buffer[0];
    figureSize = 0;
    connected = false;
    int option = 1;
    socketFD = 0;
    ConnectServer();

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
    //this->connectServer();
    if (connected == true) {
        ConnectServer();
    }
    char temp[200];
    if (option == 1) {
        Write("GET /lego/index.php HTTP/1.1\r\nhost: redes.ecci\r\n\r\n", 50, socketFD);
        connected = true;
    } else {
        Create("bufferReq.txt");
        int file = Open("bufferReq.txt");
        Write("GET /lego/list.php?figure=", 26, file);
        Write(figureName, figureSize - 1, file);
        Write(" HTTP/1.1\r\nhost: redes.ecci\r\n\r\n", 32, file);
        int size = 58 + figureSize;
        Close(file);
        file = Open("bufferReq.txt");
        Read(&temp[0], size, file);
        Write(temp,size, socketFD);
    }

    bufferId = ProcessRequest(socketFD, option);
}

int ConnectServer() {
    if (socketFD != 0) {
        Close(socketFD);
    }
    char * osn = (char *) "163.178.104.187";
    socketFD = Socket(AF_INET_NachOS, SOCK_STREAM_NachOS, 0);
    return Connect(socketFD, osn, 80);
}

int mainMenuHandle() {
    // el regex imprime las figuras
    int option;
    Write("0.Exit\nr.Refresh\nSelect option:\n", 32, 1);

    int figureAmount = 0;
    Read((char*)&figureAmount, 1, bufferId);

    option = InputRequest(0, figureAmount, 'r');
    int index = 0;
    while ( index <= option) {
        figureSize = Read(figureName, -20, bufferId);
        index++;
    }

    if (option == 0) {
        return 0;
    }

    if (option == 'r') {
        return 1;
    } 
    
    return 2;
}

int handleFigure() {
    Write("0.Close\n1.Main Menu\n2.Assemble\nr.Refresh\n", 42, 1);

    int choice = InputRequest(0, 2, 'r');
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

int handleServerRequest() {
    // connect to new socket
    int serverSocket = Socket(AF_INET_NachOS, SOCK_STREAM_NachOS, 1);

    Connect(serverSocket, "awa", PORT);

    char buffer[serverHandleBufferSize];
    int size = 0;

    char id = 1;

    Write(&id, 1, serverSocket);

    Read(
        buffer,
        -serverHandleBufferSize,
        bufferId
    );

    // while not at end of transmission
    while (buffer[0] != 4) {
        // get number from file buffer
        size = Read(
            buffer,
            -serverHandleBufferSize,
            bufferId
            );
        // send to server the piece amount
        Write(buffer, size, serverSocket);

        if (buffer[0] != 4) {
            // get piece description from file buffer
            size = Read(
                buffer,
                -(serverHandleBufferSize - size),
                bufferId
                );
            // send to server the piece description
            Write(buffer, size, serverSocket);
        }
    }

    // receive response
    char response[2];
    response[0] = '0';
    response[1] = '0';

    Read(response, 2, serverSocket);

    // generate answer
    if (response[0] == '1') {
        Write("Assembled\n", 10, 1);
    } else {
        Write("Not assembled\n", 14, 1);
    }

    return 1;
}



// processRequest syscall
// params: socket id, option, char* animal


// inputRequest syscall
