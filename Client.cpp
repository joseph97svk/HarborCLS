#include <stdio.h>
#include <string.h>
#include <regex>
#include <vector>
#include "Client.hpp"

Client::Client(char type, bool IPv6):
type(type),
IPv6(IPv6),
socket(nullptr){
}

int Client::connectServer() {
  this->socket = new Socket(this->type, this->IPv6);
  char* os = (char*) "os.ecci.ucr.ac.cr";
  char* osn = (char*) "163.178.104.187";
  return this->Connect( osn, 80 );
}

bool Client::makeRequest(std::string request) {
  if (this->connected) {
    this->Close();
    this->connectServer();
  }

  bool requestMenu = false;
  char temp[100];
  memset(temp, 0, 100);
  if (request == "menu") {
    requestMenu = true;
    strcpy(temp, "GET /lego/index.php");
    strcat(temp, " HTTP/1.1\r\nhost: redes.ecci\r\n\r\n");
    this->connected = true;
  } else {
    strcpy(temp, "GET /lego/list.php?figure=");
    strcat(temp, request.c_str());
    strcat(temp, " HTTP/1.1\r\nhost: redes.ecci\r\n\r\n");
  }

  char* requestComplete = temp;
  this->Write(requestComplete, strlen(requestComplete));
  processRequest(requestMenu);
  memset(requestComplete, 0, 100);
  return requestMenu;
}

bool Client::inAnimalArray(std::string animal) {
  bool foundAnimal = false;
  for (std::string animals : this->animalsArray) {
    if (animals == animal) {
      foundAnimal = true;
    }
  }
  return foundAnimal;
}


void Client::regexAnalyzer(bool requestMenu, std::string& line) {
if (requestMenu == true) {
    std::regex regexMenu("<OPTION\\s+value=\"(?!None\")([^\"]+)\">");
    std::smatch optionMatch;
    std::string::const_iterator begin(line.cbegin());
    if (std::regex_search(begin, line.cend(), optionMatch, regexMenu)) {
      if (inAnimalArray(optionMatch[1]) == false) {
        this->animalsArray.push_back(optionMatch[1]);
      }
      // Actualizar la posición en la cadena de respuesta
      begin = optionMatch.suffix().first;
    }
  } else {
      std::regex regexPiece("<TR><TD ALIGN=center> (\\d+)</TD>\\s*<TD ALIGN=center> ([^<]+)</TD>");

      std::smatch pieza_match;
      std::string::const_iterator begin(line.cbegin());
      if (std::regex_search(begin, line.cend(), pieza_match, regexPiece)) {
        // Obtener la cantidad, descripción
        std::string amount = pieza_match[1];
        std::string descripcion = pieza_match[2];
        // Convertir la cantidad a un entero
        int cantidad = std::stoi(amount);
        std::cout << "Cantidad: " << cantidad << ", Descripción: " << descripcion << std::endl;
        // Actualizar la posición en la cadena de respuesta
        begin = pieza_match.suffix().first;
      }
  }
}


void Client::processRequest(bool requestMenu) {
  begin:
  std::string response;
  int amountRead = 0;
  char buffer[500];
  memset(buffer, 0, 500);
  std::string lastLine = "";
  std::string line = "";
  std::string endOfDoc = "";
  int cyclesSinceEndOfBytes = 4;
  int count = 0;
  while (this->Read(buffer, 500) > 0) {
    count++;
    response.erase();
    response = buffer;
    memset(buffer, 0, sizeof(buffer));
    int character = 0;
    int initLocation = 0;

    bool endOfBytes = false;

    while (character < response.size()) {
      // check if at the end of bytes read (incomplete statement possible)
      endOfBytes = (character == response.size() - 1);

      // if whole line or end of bytes
      if(response[character] == '\n' || endOfBytes) {

        // if two turns gone by
        if (cyclesSinceEndOfBytes == 2) {
          // join saved lines with last line
          endOfDoc += lastLine;

          // set last line as all saved lines
          lastLine = endOfDoc;

          // empty previous buffer
          endOfDoc.empty();

          // restart cycle out of range
          cyclesSinceEndOfBytes = 4;
        }

        int adjustment = 1;
        // adjust location of end of string if character is invalid
        if ((u_int)(char) response[character - 1] > 127) {
          adjustment-=2;
        }
        
        // if at end of bytes
        if (endOfBytes) {
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
        regexAnalyzer(requestMenu, line);

        // set current line as last line for the next iteration to have it
        lastLine = response.substr(initLocation, character - initLocation + adjustment);

        // increase counters
        initLocation = character + 1;
        cyclesSinceEndOfBytes++;
      }
      character++;
    }    
  }
  if (requestMenu) {
    mainMenuHandle();
  } else {
    if (handleFigure()) {
      requestMenu = true;
      goto begin;
    }
  }
  
}

int inputHandler (int minRange, int maxRange) {
  std::string inputString;
  
  // while string is readable, read
  while (std::cin >> inputString) {
    bool nonValidString = false;
    // for all characters read in the string
    for (int character = 0; character < inputString.size(); character++) {
      // check if value is numeric
      if (('0' > inputString[character] ||
          inputString[character] > '9') &&
          (character == 0 && inputString[character] != '-')) {

        nonValidString = true;
      }
    }

    if (nonValidString) {
      // if not, prompt correct value
      std::cout << "\nMensaje: \"" << inputString
          << "\" no es una opcion valida. Por valor introducir un numero entre\n"
          << "[" << minRange << " y " << maxRange << "]" << std::endl;
      continue;
    }

    // change into a value
    int inputValue = atoi(inputString.c_str());

    // if within range
    if (minRange <= inputValue && inputValue <= maxRange) {
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


void Client::mainMenuHandle() {
  // if no figures were found, the page suffered an error
  if (this->animalsArray.size() == 0) {
    std::cout << "Error: la pagina no generó respuesta"
      << std::endl;
    return;
  }

  // print header
  std::cout << "Universidad de Costa Rica\n" << std::endl
      << "Escuela de Ciencias de la Computación e Informática" << std::endl
      << "CI-0123 PI de sistemas operativos y redes" << std::endl
      << "Ejemplos de servicio\n" << std::endl 
      << "Figuras:\n" << std::endl;

  // print all options
  int optionNumber = 1;
  for (const std::string& nombre : this->animalsArray) {
    std::cout << optionNumber << ".\t" << nombre << std::endl;
    optionNumber++;
  }

  // prompt an input
  std::cout << "\nSeleccione una de las opciones anteriores\n"
      << "o\n0 para cerrar el programa\n" << std::endl;
  
  // get such input
  int choice = inputHandler(0, this->animalsArray.size());

  if (choice == 0) {
    return;
  }

  std::string animalName = animalsArray[choice - 1];
  
  for (size_t character = 0;
      character < animalName.size();
      character++) {
    animalName[character] = toupper(animalName[character]);
  }
  
  std::cout << std::endl
  << "<<<" << animalName << ">>>" 
  << std::endl << std::endl;
  this->makeRequest(animalsArray[choice - 1]);
}

bool Client::handleFigure() {
  std::cout << "\n\nPara volver al menu principal: 1." << std::endl
      << "Para cerrar el programa: 0." << std::endl;
  int choice = inputHandler(0, 1);
  if (choice == 0) {
    return false;
  } 
  return true;
}

int Client::Connect(const char * host, int port) {
  return this->socket->Connect(host, port);
}
int Client::Write(const void *text, size_t size) {
  return this->socket->Write(text, size);
}
void Client::Close() {
  this->socket->Close();
}
int Client::Read(void * text, size_t size) {
  return this->socket->Read(text, size);
}