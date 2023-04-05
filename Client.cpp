#include <stdio.h>
#include <string.h>
#include <regex>
#include <vector>
#include "Client.hpp"

int Client::connectServer() {
  char* os = (char*) "os.ecci.ucr.ac.cr";
  char* osn = (char*) "163.178.104.187";
  this->InitSSL();
  return this->SSLConnect( os, 80 );
}


bool Client::makeRequest(std::string request) {
  bool requestMenu = false;
  char temp[100];
  memset(temp, 0, 100);
  if (request == "menu") {
    requestMenu = true;
    strcpy(temp, "GET /lego/index.php");
    strcat(temp, " HTTP/1.1\r\nhost: redes.ecci\r\n\r\n");
  } else {
    strcpy(temp, "GET /lego/list.php?figure=");
    strcat(temp, request.c_str());
    strcat(temp, " HTTP/1.1\r\nhost: redes.ecci\r\n\r\n");
  }
  char* requestComplete = temp;
  this->SSLWrite((void *)  requestComplete, strlen( requestComplete ) );
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
  std::string response;
  int amountRead = 0;
  char buffer[500];
  memset(buffer, 0, 500);
  std::string lastLine = "";
  std::string line = "";
  std::string endOfDoc = "";
  std::string beforeEndOfDoc = "";
  int cyclesSinceEndOfBytes = 4;

  while (this->SSLRead(buffer, 500) > 0) {
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
          // join saved lines
          beforeEndOfDoc += endOfDoc;
          // join saved lines with last line
          beforeEndOfDoc += lastLine;
          // set last line as all saved lines
          lastLine = beforeEndOfDoc;
          endOfDoc.empty();
          beforeEndOfDoc.empty(); 
          // restart cycle out of range
          cyclesSinceEndOfBytes = 4;
        }
        
        // if at end of bytes
        if (endOfBytes) {
          // copy the line before
          beforeEndOfDoc = lastLine;
          // copy actual line
          endOfDoc = response.substr(initLocation, character - initLocation + 1);
          // begin counter of cycles since found
          cyclesSinceEndOfBytes = 0;
        }

        line = lastLine + response.substr(initLocation, character - initLocation + 1);
        regexAnalyzer(requestMenu, line);

        lastLine = response.substr(initLocation, character - initLocation + 1);
        initLocation = character + 1;
        cyclesSinceEndOfBytes++;
      }
      character++;
    }    
  }
    if (requestMenu) {
      for (const std::string& nombre : this->animalsArray) {
        std::cout << nombre << std::endl;
     }
    }

}


// int Client::getFigure(std::string figure) {
//   char* os = (char*) "os.ecci.ucr.ac.cr";
//   char* osn = (char*) "163.178.104.187";
//   char temp[100];
//   // construcción de request
//   //  "GET /lego/list.php?figure="
//   // "GET /lego/index.php"
//   strcpy(temp, "GET /lego/list.php?figure=");
//   strcat(temp, figure.c_str());
//   strcat(temp, " HTTP/1.1\r\nhost: redes.ecci\r\n\r\n");
//   // para que no haya basura
//   char* request = temp;
//   char buffer[500];
//   memset(buffer, 0, 500);
//   this->Connect( osn, 80 );
//   this->Write(  request, strlen( request ) );

//   std::string response;
//   int amountRead = 0;

//   std::string lastLine = "";
//   std::string line = "";
//   std::string endOfDoc = "";
//   std::string beforeEndOfDoc = "";
//   std::vector<std::string> animalsArray;
//   int cyclesSinceEndOfBytes = 4;

//   while (this->Read(buffer, 500) > 0) {
//     response.erase();
//     response = buffer;
//     memset(buffer, 0, sizeof(buffer));
//     int character = 0;
//     int initLocation = 0;

//     bool endOfBytes = false;

//     while (character < response.size()) {
//       // check if at the end of bytes read (incomplete statement possible)
//       endOfBytes = (character == response.size() - 1);

//       // if whole line or end of bytes
//       if(response[character] == '\n' || endOfBytes) {

//         // if two turns gone by
//         if (cyclesSinceEndOfBytes == 2) {
//           // join saved lines
//           beforeEndOfDoc += endOfDoc;
//           // join saved lines with last line
//           beforeEndOfDoc += lastLine;
//           // set last line as all saved lines
//           lastLine = beforeEndOfDoc;
//           endOfDoc.empty();
//           beforeEndOfDoc.empty(); 
//           // restart cycle out of range
//           cyclesSinceEndOfBytes = 4;
//         }
        
//         // if at end of bytes
//         if (endOfBytes) {
//           // copy the line before
//           beforeEndOfDoc = lastLine;
//           // copy actual line
//           endOfDoc = response.substr(initLocation, character - initLocation + 1);
//           // begin counter of cycles since found
//           cyclesSinceEndOfBytes = 0;
//         }

//         line = lastLine + response.substr(initLocation, character - initLocation + 1);
//         // std::regex regexMenu("<OPTION\\s+value=\"(?!None\")([^\"]+)\">");
//         // std::smatch optionMatch;
//         // std::string::const_iterator begin(line.cbegin());
//         // if (std::regex_search(begin, line.cend(), optionMatch, regexMenu)) {
//         //   if (inAnimalArray(animalsArray, optionMatch[1]) == false) {
//         //     animalsArray.push_back(optionMatch[1]);
//         //   }
//         //   // Actualizar la posición en la cadena de respuesta
//         //   begin = optionMatch.suffix().first;
//         // }

//         std::regex regexPiece("<TR><TD ALIGN=center> (\\d+)</TD>\\s*<TD ALIGN=center> ([^<]+)</TD>");

//         std::smatch pieza_match;
//         std::string::const_iterator begin(line.cbegin());
//         if (std::regex_search(begin, line.cend(), pieza_match, regexPiece)) {
//           // Obtener la cantidad, descripción
//           std::string amount = pieza_match[1];
//           std::string descripcion = pieza_match[2];
//           // Convertir la cantidad a un entero
//           int cantidad = std::stoi(amount);
//           std::cout << "Cantidad: " << cantidad << ", Descripción: " << descripcion << std::endl;
//           // Actualizar la posición en la cadena de respuesta
//           begin = pieza_match.suffix().first;
//         }
        
//         lastLine = response.substr(initLocation, character - initLocation + 1);
//         initLocation = character + 1;
//         cyclesSinceEndOfBytes++;
//       }
//       character++;
//     }    
//   }

//     for (const std::string& nombre : animalsArray) {
//       std::cout << nombre << std::endl;
//    }

//   return 0;
// }
