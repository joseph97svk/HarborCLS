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
   while (this->Read(buffer, 500) > 0) {
     response.erase();
     response = buffer;
     memset(buffer, 0, sizeof(buffer));
        // Regex para encontrar las piezas
    std::regex regexPiece("<TR><TD ALIGN=center> (\\d+)</TD>\\s*<TD ALIGN=center> ([^<]+)</TD>");

    std::smatch pieza_match;
    std::string::const_iterator begin(response.cbegin());
    while (std::regex_search(begin, response.cend(), pieza_match, regexPiece) ) {
      // Obtener la cantidad, descripción
      std::string amount = pieza_match[1];
      std::string descripcion = pieza_match[2];
      // Convertir la cantidad a un entero
      int cantidad = std::stoi(amount);

      std::cout << "Cantidad: " << cantidad << ", Descripción: " << descripcion  << std::endl;
      // Actualizar la posición en la cadena de respuesta
      begin = pieza_match.suffix().first;
    }

   }

 

   return 0;
}
