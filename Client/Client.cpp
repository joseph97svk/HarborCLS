#include <stdio.h>
#include <string.h>
#include <regex>
#include <vector>
#include "Client.hpp"
#include <unistd.h>
#include <iomanip>

/**
 * Client method
 * Constructor for the Client class. Initializes the object with the provided type and IPv6
 * values, and sets the socket pointer to nullptr.
 *
 * @param type The type of the client.
 * @param IPv6 Whether the client uses IPv6.
 *
 **/
Client::Client(char type, bool IPv6) : type(type),
                                       IPv6(IPv6),
                                       socket(nullptr)
{
}

/**
 * connectServer method
 * This function creates a new socket connection to the server IP address specified in the code.
 *
 * @return int returns 0 if the connection was successful, or a negative number if it failed.
 *
 **/
int Client::connectServer()
{
  if (this->socket != nullptr)
  {
    this->Close();
  }

  // Create a new socket object with the specified protocol type and IP version.
  this->socket = new Socket(this->type, this->IPv6);

  char *osn = (char *)"163.178.104.187";
  this->socket->SSLInit();
  return this->Connect(osn, (char *)"https");
}

int Client::run(RequestType initOption)
{
  RequestType option = initOption;
  while (option != RequestType::Exit)
  {
    if (option == RequestType::MenuRequest)
    {
      makeRequest(option);
      option = mainMenuHandle();
    }
    else if (option == RequestType::FigureRequest)
    {
      makeRequest(option);
      option = handleFigure();
    }
    else if (option == RequestType::Server)
    {
      // server
      std::cout << "Enter server" << std::endl;
      option = handleServerRequest();
    }
  }
  //_exit( 0 );
  return 0;
}

/**
 * makeRequest method
 * This method makes an HTTP GET request to the server.
 *
 * @param request The request to be made. If it is "menu", the request will retrieve the menu options.
 * @return A boolean indicating whether the request made was for the menu or not.
 *
 **/
bool Client::makeRequest(RequestType requestType)
{
  // Check if client is already connected to server
  if (this->connected)
  {
    // If not, attempt to connect to server
    this->connectServer();
  }

  char temp[100];
  memset(temp, 0, 100);
  if (requestType == RequestType::MenuRequest)
  {
    // Request for menu
    strcpy(temp, "GET /lego/index.php");
    strcat(temp, " HTTP/1.1\r\nhost: redes.ecci\r\n\r\n");
    this->connected = true;
  }
  else
  {
    // Request for a specific figure
    strcpy(temp, "GET /lego/list.php?figure=");
    strcat(temp, this->currentFigure.c_str());
    strcat(temp, " HTTP/1.1\r\nhost: redes.ecci\r\n\r\n");
  }

  // Send request to server
  char *requestComplete = temp;
  this->Write(requestComplete, strlen(requestComplete));

  // Process server response
  processRequest(requestType);
  memset(requestComplete, 0, 100);
  return requestType == RequestType::MenuRequest;
}

/**
 * in figuresArray method
 * Checks if the given figures is already present in the client's figures.
 *
 * @param figure the figure to check for.
 * @return true if the figure is already present in the figures, false otherwise.
 *
 **/
bool Client::inFigureArray(std::string figure)
{
  bool foundFigure = false;
  for (std::string figures : this->figuresArray)
  {
    if (figures == figure)
    {
      foundFigure = true;
    }
  }
  return foundFigure;
}

/**
 * regexAnalyzer method
 * This method analyzes a given line using regular expressions to extract information depending
 *
 * @param requestMenu a boolean indicating whether a menu request is being made or not.
 * @param line a reference to a string representing the line to be analyzed.
 * @param totalAmount a reference to an integer representing the total amount of pieces.
 *
 **/
void Client::regexAnalyzer(RequestType requestType, std::string &line, int &totalAmount)
{
  if (requestType == RequestType::MenuRequest)
  {
    // Regex to match menu options
    std::regex regexMenu("<OPTION\\s+value=\"(?!None\")([^\"]+)\">");
    std::smatch optionMatch;
    std::string::const_iterator begin(line.cbegin());
    if (std::regex_search(begin, line.cend(), optionMatch, regexMenu))
    {
      if (inFigureArray(optionMatch[1]) == false)
      {
        this->figuresArray.push_back(optionMatch[1]);
      }
      // Update position in the response string
      begin = optionMatch.suffix().first;
    }
  }
  else
  {
    // Regex to match pieces in figure response
    std::regex regexPiece("<TR><TD ALIGN=center?> (\\d+)</TD>\\s*<TD ALIGN=center> ([^<]+)</TD>");
    std::smatch pieza_match;
    std::string::const_iterator begin(line.cbegin());

    if (std::regex_search(begin, line.cend(), pieza_match, regexPiece)) {
      // Extract quantity and description of the piece
      std::string amount = pieza_match[1];
      std::string descripcion = pieza_match[2];
      // Convert quantity to integer
      int cantidad = std::stoi(amount);
      std::cout << std::left << std::setw(15) << cantidad << descripcion << std::endl;
      this->requestedPieces.emplace_back(descripcion, cantidad);
      totalAmount += cantidad;
      // Update position in the response string
      begin = pieza_match.suffix().first;
    }
  }
}


/**
 * processRequest method
 * This method processes a request from a client, reading a response from a buffer.
 *
 * @param requestMenu A boolean indicating whether the menu should be requested.
 *
 **/
void Client::processRequest(RequestType requestType)
{
  // begin:
  std::string response;
  int totalAmount = 0;
  char buffer[501];
  memset(buffer, 0, 501);
  std::string lastLine = "";
  std::string line = "";
  std::string endOfDoc = "";
  int cyclesSinceEndOfBytes = 4;
  int count = 0;

  if (requestType == RequestType::FigureRequest)
  {
    std::cout << std::left << std::setw(15) << "Cantidad"
              << "Descripción" << std::endl;
    std::cout << std::left << std::setw(15) << "--------"
              << "-----------" << std::endl;
  }

  while (this->Read(buffer, 500) > 0)
  {
    count++;
    response.erase();
    response.resize(strlen(buffer));
    response = buffer;
    size_t bufferSize = strlen(buffer);
    memset(buffer, 0, sizeof(buffer));
    size_t character = 0;
    size_t initLocation = 0;

    bool endOfBytes = false;

    while (character < response.size())
    {
      // check if at the end of bytes read (incomplete statement possible)
      endOfBytes = (character == response.size() - 1);

      // if whole line or end of bytes
      if (response[character] == '\n' || endOfBytes)
      {

        // if two turns gone by
        if (cyclesSinceEndOfBytes == 2)
        {
          // join saved lines with last line
          endOfDoc += lastLine;

          // set last line as all saved lines
          lastLine = endOfDoc;

          size_t emptyTries = 0;
          // empty previous buffer
          while (endOfDoc.empty() && emptyTries < 10)
          {
            emptyTries++;
          }

          // restart cycle out of range
          cyclesSinceEndOfBytes = 4;
        }

        int adjustment = 1;
        // adjust location of end of string if character is invalid
        if ((u_int)(char)response[character - 1] > 127)
        {
          adjustment -= 2;
        }

        // if at end of bytes
        if (endOfBytes)
        {
          // copy previous and actual lines
          endOfDoc = lastLine +
                     response.substr(initLocation, character - initLocation + adjustment);
          // begin counter of cycles since found
          cyclesSinceEndOfBytes = 0;
        }

        /*analyze last line and this line together
        (some regex may need to analyze two lines at once) */
        line = lastLine +
               response.substr(initLocation, character - initLocation + adjustment);

        // run regex to analyze line
        regexAnalyzer(requestType, line, totalAmount);

        // set current line as last line for the next iteration to have it
        lastLine = response.substr(initLocation, character - initLocation + adjustment);

        // increase counters
        initLocation = character + 1;
        cyclesSinceEndOfBytes++;
      }
      character++;
    }

    if (requestType == RequestType::Server
        &&  bufferSize != 500) {
      break;
    }
  }
}

/**
 * inputHandler method
 *   This function handles user input from the standard input, checking if it is within the
 *   specified range and if it is a valid numeric value. It also allows for a single character
 *   exception to be passed, in which case it returns that character directly.
 * @param	minRange The minimum acceptable numeric value for the input.
 * @param	maxRange The maximum acceptable numeric value for the input.
 * @param exception A single character exception that can be passed to return that character directly.
 * @return The validated input value as an integer.
 *
 **/
int inputHandler(int minRange = 0, int maxRange = 1, char exception = '\0')
{
  std::string inputString;

  // while string is readable, read
  while (std::cin >> inputString)
  {
    bool nonValidString = false;

    // Check if exception character is present and return it directly
    if (exception != '\0' && inputString[0] == exception && inputString.size() == 1)
    {
      return (int)inputString[0];
    }

    // for all characters read in the string
    for (size_t character = 0; character < inputString.size(); character++)
    {
      // check if value is numeric
      if (('0' > inputString[character] ||
           inputString[character] > '9') &&
          (character == 0 && inputString[character] != '-'))
      {

        nonValidString = true;
      }
    }

    // If the input string is not a valid number, prompt the user to enter a valid number
    if (nonValidString)
    {
      // if not, prompt correct value
      std::cout << "\nMensaje: \"" << inputString
                << "\" no es una opcion valida. Por valor introducir un numero entre\n"
                << "[" << minRange << " y " << maxRange << "]" << std::endl;
      continue;
    }

    // change into a value
    int inputValue = atoi(inputString.c_str());

    // if within range
    if (minRange <= inputValue && inputValue <= maxRange)
    {
      // return value
      return inputValue;
    }
    std::cout << "\nMensaje: " << inputValue
              << " no esta dentro del rango, por favor introducir un valor entre\n"
              << "[" << minRange << " y " << maxRange << "]" << std::endl;
  }

  // if input was not readable, return error
  return -2;
}

/**
 * mainMenuHandle method
 * Handle the main menu functionality, which prints the available options and prompts the user to select one.
 * If no figures were found, the function prints an error message and returns 0.
 * @return 0 if a valid figure option was chosen, or 1 if a refresh is requested.
 *
 **/
RequestType Client::mainMenuHandle()
{
  // if no figures were found, the page suffered an error
  /* if (this->figuresArray.size() == 0)
  {
    std::cout << "Error: la pagina no generó respuesta"
              << std::endl;
    return RequestType::Base;
  } */

  // print header
  std::cout << "Universidad de Costa Rica\n"
            << std::endl
            << "Escuela de Ciencias de la Computación e Informática" << std::endl
            << "CI-0123 PI de sistemas operativos y redes" << std::endl
            << "Ejemplos de servicio\n"
            << std::endl
            << "Figuras:\n"
            << std::endl;

  // print all options
  int optionNumber = 1;
  for (const std::string &nombre : this->figuresArray)
  {
    std::cout << optionNumber << ".\t" << nombre << std::endl;
    optionNumber++;
  }

  // prompt an input
  std::cout << "\nSeleccione una de las opciones anteriores\n"
            << "\to\n0 para cerrar el programa\n"
            << "\to\n'r' para refrescar (el servidor puede entregar datos incorrectamente generando comportamiento indefinido"
            << ",\nsu este es el caso, entonces refresque la pagina)\n"
            << std::endl;

  // get such input
  int choice = inputHandler(0, this->figuresArray.size(), 'r');

  if (choice == 0)
  {
    return RequestType::Exit;
  }

  if (choice == 'r')
  {
    return RequestType::MenuRequest;
  }

  std::string figureName = figuresArray[choice - 1];

  for (size_t character = 0;
       character < figureName.size();
       character++)
  {
    figureName[character] = toupper(figureName[character]);
  }

  this->currentFigure = figuresArray[choice - 1];

  std::cout << std::endl
            << "<<<" << figureName << ">>>"
            << std::endl
            << std::endl;
  return RequestType::FigureRequest;
}

/**
 * handleFigure method
 *  responsible for handling the user input when a figure (figure) is displayed on the screen.
 *  @return 1 when the method has completed its execution successfully
 **/
RequestType Client::handleFigure()
{

  // Display a menu to the user with options to return to the main menu, exit the program, or refresh the current figure
  std::cout << "\n\nPara armar la figura: 2." << std::endl
            << "Para volver al menu principal: 1." << std::endl
            << "Para cerrar el programa: 0."
            << "\n\to\n'r' para refrescar (el servidor puede entregar datos incorrectamente generando comportamiento indefinido"
            << ",\nsu este es el caso, entonces refresque la pagina)\n"
            << std::endl;

  // Handle the user's input
  int choice = inputHandler(0, 2, 'r');
  if (choice == 0)
  {
    return RequestType::Exit;
  }

  if (choice == 'r')
  {
    // If the user chooses to refresh the current figure, update the displayed figure by making a new request to the server
    std::string figureName = this->currentFigure;

    for (size_t character = 0;
         character < figureName.size();
         character++)
    {
      figureName[character] = toupper(this->currentFigure[character]);
    }

    std::cout << std::endl
              << "<<<" << figureName << ">>>"
              << std::endl
              << std::endl;
    return FigureRequest;
  }

  if (choice == 2) {
    return RequestType::Server;
  }

  return RequestType::MenuRequest;
}

/**
 * Connect method
 *   Makes a call to the Connect method inside the Socket class
 *   which make use of the "connect" Unix system call.
 *
 * @param	char * host: host address in dot notation, example "10.1.104.187"
 * @param	int port: process address, example 80
 *
 **/
int Client::Connect(const char *host, int port)
{
  return this->socket->SSLConnect((char*) host, port);
}

/**
 * Connect method
 *   Makes a call to the Connect method inside the Socket class
 *   which make use of the "connect" Unix system call.
 *
 * @param	char * host: host address in dot notation, example "10.1.104.187"
 * @param	int port: process address, example 80
 *
 **/
int Client::Connect(const char *host, const char *service)
{
  return this->socket->SSLConnect((char*) host, (char*) service);
}

/**
 * Write method
 * Makes a call to the Write method inside the Socket class
 * which use "write" Unix system call (man 3 write)
 *
 * @param	void * buffer: buffer to store data write to socket
 * @param	size_t size: buffer capacity, number of bytes to write
 *
 **/
int Client::Write(const void *text, size_t size)
{
  return this->socket->SSLWrite(text, size);
}

/**
 * Close method
 * This method is responsible for closing the client's socket connection
 * and freeing up the memory allocated for the socket object.
 **/
void Client::Close()
{
  // this->socket->Close();
  delete this->socket;
}

/**
 * Read method
 * This method is responsible for reading data from the client's socket
 * connection.
 *
 * @param	void * text: buffer to store data read from socket
 * @param	int size: buffer capacity, read will stop if buffer is full
 *
 **/
int Client::Read(void *text, size_t size)
{
  return this->socket->SSLRead(text, size);
}

void Client::showPiecesServer() {
 

  this->processRequest(RequestType::Server);
}

RequestType Client::handleServerRequest() {
  std::string response = "";

  Socket* socketBuffer = this->socket;

  this->socket = new Socket('s', false);
  this->socket->InitSSL();

  this->socket->SSLConnect("ip address in dot decimal format", 2816); // Same port as server

  for (int piece = 0; piece < this->requestedPieces.size(); piece++) {
    response.append("<TR><TD ALIGN=center> ");
    response.append(this->requestedPieces[piece].first);
    response.append(
          "</TD>\n"
          "<TD ALIGN=center> ");
    response.append(std::to_string(this->requestedPieces[piece].second));
    response.append(
      "</TD>\n"
      "</TR>\n"); 
  }

  this->Write(response.c_str(), response.size());

  char buffer[501];
  memset(buffer, 0, 501);
  
  std::cout << "reading response" << std::endl;

  processRequest(RequestType::Server); 

  bool figureAssembled = (bool)(int) buffer[0];
  if (figureAssembled) {
    std::cout << "Your figure has been assembled!" << std::endl;
  } else {
    std::cout << "There were not enough pieces to assemble your figure :'(" << std::endl;
  }

  this->socket = socketBuffer;

  return RequestType::MenuRequest;
}