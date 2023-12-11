#include "syscall.h"

// works with own server, message displayed in server screen
int main() {
   int id;
   char a[ 512 ];

   Write("awawawawawa!\n", 13, 1);

   id = Socket( AF_INET_NachOS, SOCK_STREAM_NachOS, 1);
   Connect( id, "ip address in dot decimal format", 2816 );
   Write( "GET / HTTP/1.0\r\n\r\n", 32, id );

   Write( "GET / HTTP/1.0\r\n\r\n", 32, id );

   Read( a, 2, id );

   if (a[0] == 0) {
      Write("Funca", 5, 1 );
   } else {
      Write("awawawawawa!\n", 13, 1);
   }

   

   //Write( a, 500, 1 );
  
   Close( id );

}
