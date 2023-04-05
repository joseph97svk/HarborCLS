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

     int character = 0;

     int initLocation = 0;

     while(character < response.size()){
      if(response[character] == '\n') {
        std::string line = response.substr(initLocation, character - initLocation);
        initLocation = character + 1;

        std::regex regexPieceAmount("<TR><TD ALIGN=center> (\\d+)</TD>");
        std::regex regexPiece("<TD ALIGN=center> ([^<]+)</TD>");

        std::smatch pieza_match;
        std::string::const_iterator begin(line.cbegin());

        bool foundAmount = false;

        std::string::const_iterator beginNum(line.cbegin());
        if (std::regex_search(begin, line.cend(), pieza_match, regexPieceAmount)) {
          std::string amount = pieza_match[1];
          int cantidad = std::stoi(amount);
          std::cout << "Cantidad: " << cantidad;
          beginNum = pieza_match.suffix().first;
          foundAmount = true;
        }
         if (std::regex_search(begin, line.cend(), pieza_match, regexPiece) ) {
          if (!foundAmount) {
            std::string descripcion = pieza_match[1];
            std::cout << ", Descripción: " << descripcion << std::endl;
            begin = pieza_match.suffix().first;
            }
          }
       // std::cout << "line change" << std::endl;
      }
      character++;
     }    
   }

   return 0;
}
