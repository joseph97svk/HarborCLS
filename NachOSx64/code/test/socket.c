#include "syscall.h"

int main() {
   int id;
   char a[ 512 ];

   Write("awawawawawa\n", 12, 1);

   id = Socket( AF_INET_NachOS, SOCK_STREAM_NachOS, 0);
   Connect( id, "163.178.104.187", 80 );
   Write( "GET / HTTP/1.0\r\n\r\n", 32, id );
   Read( a, 512, id );
   Write( a, 512, 1 );
   Close( id );
}
