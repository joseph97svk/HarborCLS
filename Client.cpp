#include <stdio.h>
#include <string.h>
#include <regex>
#include "Client.hpp"

int Client::getFigure(std::string figure) {
  char* os = (char*) "os.ecci.ucr.ac.cr";
  char* osn = (char*) "163.178.104.187";
  char temp[100];
  // construcción de request
  strcpy(temp, "GET /lego/list.php?figure=");
  strcat(temp, figure.c_str());
  strcat(temp, " HTTP/1.1\r\nhost: redes.ecci\r\n\r\n");
  // para que no haya basura
  char* request = temp;
  char buffer[500];
  memset(buffer, 0, 500);
  this->Connect( osn, 80 );
  this->Write(  request, strlen( request ) );

  std::string response;
  int amountRead = 0;

  std::string lastLine = "";
  std::string line = "";
  std::string endOfDoc = "";
  std::string beforeEndOfDoc = "";

  int cyclesSinceEndOfBytes = 4;

  while (this->Read(buffer, 500) > 0) {
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
        
        lastLine = response.substr(initLocation, character - initLocation + 1);
        initLocation = character + 1;
        cyclesSinceEndOfBytes++;
      }
      character++;
    }    
  }

  return 0;
}
