// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
// For now, this only handles the Halt() system call.
// Everything else core dumps.
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#include "addrspace.h"

#include <fcntl.h> // for unix open
#include <unistd.h> // for unix write
#include <iostream> // for debugging printing

// for socket
#include <sys/socket.h>
#include <socketLib.h>

#include <stdio.h>
#include <string.h>
#include <regex>
#include <vector>
#include <iomanip>


#define openFilesTable currentThread->space->openFilesTable
#define processTable (*processTable)

void returnFromSystemCall () {
   // get current program counter
   int programCounter = machine->ReadRegister(PCReg);
   // set it as past program counter
   machine->WriteRegister(PrevPCReg, programCounter);

   // get next program counter
   programCounter = machine->ReadRegister(NextPCReg);
   // set it as current program counter
   machine->WriteRegister(PCReg, programCounter);

   // increase new current program counter
   programCounter += 4;
   // set it as the next one
   machine->WriteRegister(NextPCReg, programCounter);
}

// Pass the user routine address as a parameter for this function
// This function is similar to "StartProcess" in "progtest.cc" file under "userprog"
// Requires a correct AddrSpace setup to work well

void NachosForkThread( void * p ) { // for 64 bits version
   AddrSpace *space;

   space = currentThread->space;
   space->InitRegisters();             // set the initial register values
   space->RestoreState();              // load page table register
   openFilesTable->addThread();

   // Set the return address for this thread to the same as the main thread
   // This will lead this thread to call the exit system call and finish
   machine->WriteRegister( RetAddrReg, 4 );

   machine->WriteRegister( PCReg, (long) p );
   machine->WriteRegister( NextPCReg, (long) p + 4 );

   machine->Run();                     // jump to the user progam
   ASSERT(false);
}

// same from progtest
// incluiding here because linker is a pain
// ciclical compiling and includes are too much to deal with
void startProcess(void* fileName) {
   char* filename = (char*) fileName;
   OpenFile *executable = fileSystem->Open(filename);
   AddrSpace *space;

   if (executable == NULL) {
      printf("Unable to open file %s\n", filename);
      return;
   }
   space = new AddrSpace(executable);    
   currentThread->space = space;

   delete executable;			// close file

   space->InitRegisters();		// set the initial register values
   space->RestoreState();		// load page table register

   machine->Run();			// jump to the user progam
   ASSERT(false);			// machine->Run never returns;
}

/*
 *  System call interface: Halt()
 */
void NachOS_Halt() {		// System call 0
   currentThread->Finish();
   interrupt->Halt();
}


/*
 *  System call interface: void Exit( int )
 */
void NachOS_Exit() {		// System call 1
   int returnValue = machine->ReadRegister(4);
   SpaceId spaceId = currentThread->spaceId;
   nchs::ProcessInfo* processInfo = processTable[spaceId];

   if (processInfo != nullptr) {
      Semaphore* sem = processInfo->sem;
      if (sem != nullptr) {
         sem->V();
         processTable[spaceId]->returnVal = returnValue;
      }
   }

   delete currentThread->space;

   currentThread->Yield();
   currentThread->Finish();
}

char* getText(int memLocation) {
   char* text = new char[100];

   char current = 1;
   int charPos = 0;
   int position = 0;

   memset(text, 0, 100);

   // get fileName to open
   while (current != 0) {
      charPos = position;
      machine->ReadMem(memLocation + position, 1, (int*) &current);
      position = charPos;
      if (current != 0) {
         text[position] = current;
      }
      ++position;
   };

   return text;
}


/*
 *  System call interface: SpaceId Exec( char * )
 */
void NachOS_Exec() {		// System call 2
   Thread * newT = new Thread("child to execute Fork code");

   int memLocation = machine->ReadRegister(4);

   char* fileName = getText(memLocation);

   int spaceId = processTable.insert(
      new nchs::ProcessInfo(
         new Semaphore("new exec process", 0)));
   newT->spaceId = spaceId;

   newT->Fork(startProcess, (void*) fileName);

   currentThread->Yield();

   machine->WriteRegister(2, spaceId);

   returnFromSystemCall();
}


/*
 *  System call interface: int Join( SpaceId )
 */
void NachOS_Join() {		// System call 3
   SpaceId spaceId = machine->ReadRegister(4);

   nchs::ProcessInfo* processInfo = processTable[spaceId];

   int returnValue = 0;

   if (processInfo != nullptr) {
      Semaphore* sem = processInfo->sem;
      sem->P();
      returnValue = processTable[spaceId]->returnVal;
      processTable.erase(spaceId);
   }

   machine->WriteRegister(2, returnValue);
   returnFromSystemCall();
}


/*
 *  System call interface: void Create( char * )
 */
void NachOS_Create() {		// System call 4
   int fileNameAddress = machine->ReadRegister(4);

   int nameCharBuffer = 0;
   int charOnNamePos = 0;

   char* fileName =  new char[200]; // name size chosen arbitrarily
   int dynamicStringCapacity = 200;

   do {
      // adjust name buffer size if max is reached
      if (charOnNamePos == dynamicStringCapacity - 1) {
         char* fileNameBuffer = fileName;
         dynamicStringCapacity *= 2;
         fileName = new char[dynamicStringCapacity];
         memcpy(fileName, fileNameBuffer, charOnNamePos);
         delete [] fileNameBuffer;
      }

      // add char to the name
      machine->ReadMem(fileNameAddress + charOnNamePos,
         1,
         &nameCharBuffer);

      // if not end of file
      if (nameCharBuffer != 0) {
         // copy char onto name string
         fileName[charOnNamePos] = (char) nameCharBuffer;
      }

      // increase position of next char to check
      charOnNamePos++;
   } while (nameCharBuffer != 0); // ends on end of file character

   OpenFileId fileId = creat(fileName, 0777);

   if (fileId == -1) {
      machine->WriteRegister(2, -1);
   } else {
      machine->WriteRegister(2, fileId);
   }

   delete [] fileName;

   returnFromSystemCall();
}


/*
 *  System call interface: OpenFileId Open( char * )
 */
void NachOS_Open() {		// System call 5
   // Read the name from the user memory, see 5 below
   int fileNameAddress = machine->ReadRegister(4);

   int nameCharBuffer = 0;
   int charOnNamePos = 0;

   char* fileName =  new char[200]; // name size chosen arbitrarily
   int dynamicStringCapacity = 200;

   do {
      // adjust name buffer size if max is reached
      if (charOnNamePos == dynamicStringCapacity - 1) {
         char* fileNameBuffer = fileName;
         dynamicStringCapacity *= 2;
         fileName = new char[dynamicStringCapacity];
         memcpy(fileName, fileNameBuffer, charOnNamePos);
         delete [] fileNameBuffer;
      }   

      // add char to the name
      machine->ReadMem(fileNameAddress + charOnNamePos,
         1,
         &nameCharBuffer);

      // if not end of file
      if (nameCharBuffer != 0) {
         // copy char onto name string
         fileName[charOnNamePos] = (char) nameCharBuffer;
      }

      // increase position of next char to check
      charOnNamePos++;
   } while (nameCharBuffer != 0); // ends on end of file character

   // between user file and unix file
   OpenFileId openFileId = (OpenFileId) open(fileName, O_RDWR | O_APPEND);

   if (openFileId != -1) {
      // Use NachosOpenFilesTable class to create a relationship
      openFileId = openFilesTable->Open(openFileId);

      // Verify for errors
      // if there is an error, openFileId is -1, which is already an error message
      // write the OpenFileId on return register
      machine->WriteRegister(2, openFileId);
   } else { // if open did not work
      // report failure
      machine->WriteRegister(2, -1);
   }

   delete [] fileName;
   returnFromSystemCall();		// Update the PC registers
}


/*
 *  System call interface: OpenFileId Write( char *, int, OpenFileId )
 */
void NachOS_Write() {		// System call 6
   int bufferAddr = machine->ReadRegister(4);
   int size = machine->ReadRegister(5);
   OpenFileId descriptorFile = machine->ReadRegister(6);
   char* writeBuffer = new char[size + 1];
   memset(writeBuffer, 0, size + 1);
   char buffer = 0;
   int posBuffer = 0;

   // semaphore needed?
   for (int charPos = 0; charPos < size; charPos++) {
      posBuffer = charPos;
      machine->ReadMem(bufferAddr + charPos, 1, (int*) &buffer);
      charPos = posBuffer;
      if (buffer != 0) {
         writeBuffer[charPos] = buffer;
      }
   }

   switch (descriptorFile) {
      case CONSOLE_INPUT:
         // report error as it should not be possible
         machine->WriteRegister(2, -1);
         break;

      case CONSOLE_OUTPUT:
         // just write on console
         printf("%s\n", writeBuffer);

         machine->WriteRegister(2, size);
         break;

      default:
         // Verify if the file is opened
         if (openFilesTable->isOpened(descriptorFile)) {
             // Get the unix handle from our table for open files
            int unixHandle = openFilesTable->getUnixHandle(descriptorFile);
            if (systemSockets->count(unixHandle) &&
               (*systemSockets)[unixHandle]->SSLChoice) {
               // read through SSL
               SSLWrite(
                  (*systemSockets)[descriptorFile]->SSLStruct,
                  writeBuffer,
                  size
               );
               goto doneWriting;
            }

            // Do the write to the already opened Unix file
            write(unixHandle, writeBuffer, size);

            doneWriting:

            // Return the number of chars written to user, via r2
            machine->WriteRegister(2, size);
         } else { // if not 
            // return -1 in r2
            machine->WriteRegister(2, -1);
         }
      
         break;
   }
   // Update simulation stats, see details in Statistics class in machine/stats.cc
	// Console->V();

   delete [] writeBuffer;

   returnFromSystemCall();
}


/*
 *  System call interface: OpenFileId Read( char *, int, OpenFileId )
 */
void NachOS_Read() {		// System call 7
   int bufferAddr = machine->ReadRegister(4); // get the address of the read buffer
   int size = machine->ReadRegister(5); // get the size of the buffer to be read
   OpenFileId descriptorFile = machine->ReadRegister(6); // Get the id of the file to be read
   
   int bufferSize = size;

   if (bufferSize < 0) {
      bufferSize *= -1;
   }
   
   char* readBuffer = new char[bufferSize + 1];
   //int bytesRead = 0;

   switch (descriptorFile) {
      case CONSOLE_INPUT: {

         // read from console
         //scanf("%s", readBuffer);

         char buffer = 1;
         int position = 0;
         while (buffer != 0 && position < size) {
            read(1, &buffer, 1);
            if (buffer > 8) {
               readBuffer[position] = buffer;
            }
            position++;
         }

         if (position != size) {
            readBuffer[position] = '\n';
         }

         int readChar = 0;
         // copy from read buffer into memory
         for (; 
            readChar < size // as long as within size
            || readChar < (int) strnlen(readBuffer, size) // still a string
            || readBuffer[readChar] != 0; // not end of file
            readChar++) {
            // write onto memory
            machine->WriteMem(bufferAddr + readChar, // at address position
               1, // with size 1
               readBuffer[readChar]); // from buffer at right position
         }

         machine->WriteRegister(2, readChar);
         break;
      }
      case CONSOLE_OUTPUT:
         // report error as it should not be possible
         machine->WriteRegister(2, -1);
         break;

      default:  // Should be able to read any other file
         // check if file is open
         if (openFilesTable->isOpened(descriptorFile)) {
            int unixHandle = openFilesTable->getUnixHandle(descriptorFile);
   
            int bytesRead = 0;
            if (systemSockets->count(unixHandle) &&
               (*systemSockets)[unixHandle]->SSLChoice) {
               // read through SSL
               bytesRead = SSLRead(
                  (*systemSockets)[descriptorFile]->SSLStruct,
                  readBuffer,
                  size
               );
               goto doneReading;
            }

            if (size < 0) {
               size *= -1;
               bytesRead = getline(readBuffer, size, unixHandle);
            } else {
               // read unix file
               bytesRead = read(
                  unixHandle,
                  readBuffer,
                  size
               );
            }

            

            // for all bytes read
            for (int charPos = 0; charPos < bytesRead; charPos++) {
               // copy string char onto buffer on memory
               machine->WriteMem(bufferAddr + charPos,
               1,
               readBuffer[charPos]);
            }

            doneReading:

            //report on bytes read
            machine->WriteRegister(2, bytesRead);
         } else {
            // report on file not opened
            machine->WriteRegister(2, -1);
         }
         break;
   }

   returnFromSystemCall();
}


/*
 *  System call interface: void Close( OpenFileId )
 */
void NachOS_Close() {		// System call 8
   OpenFileId fileId = machine->ReadRegister(4);
   int unixHandle = openFilesTable->Close(fileId);

   if (unixHandle == -1) {
      // report file not opened
      machine->WriteRegister(2, -1);
      returnFromSystemCall();
      return;
   }

   int result = close(unixHandle);

   if (result == 0) {
      result++;
   }

   machine->WriteRegister(2, result);
   returnFromSystemCall();
}


/*
 *  System call interface: void Fork( void (*func)() )
 */
void NachOS_Fork() {		// System call 9
   DEBUG('u', "Entering Fork System call\n");

	// We need to create a new kernel thread to execute the user thread
	Thread * newT = new Thread("child to execute Fork code");

	// We need to share the Open File Table structure with this new child

	// Child and father will also share the same address space, except for the stack
	// Text, init data and uninit data are shared, a new stack area must be created
	// for the new child
	// We suggest the use of a new constructor in AddrSpace class,
	// This new constructor will copy the shared segments (space variable) from currentThread, passed
	// as a parameter, and create a new stack for the new child
	newT->space = new AddrSpace(currentThread->space);

	// We (kernel)-Fork to a new method to execute the child code
	// Pass the user routine address, now in register 4, as a parameter
	// Note: in 64 bits register 4 need to be casted to (void *)
	newT->Fork(NachosForkThread, (void*)(size_t) machine->ReadRegister(4));

	returnFromSystemCall();	// This adjust the PrevPC, PC, and NextPC registers

	DEBUG('u', "Exiting Fork System call\n");
}


/*
 *  System call interface: void Yield()
 */
void NachOS_Yield() {		// System call 10
   currentThread->Yield();
   returnFromSystemCall();	// This adjust the PrevPC, PC, and NextPC registers
}


/*
 *  System call interface: Sem_t SemCreate( int )
 */
void NachOS_SemCreate() {		// System call 11
   int initialValue = machine->ReadRegister(4);

   // set index of semaphore as its name
   char* semName = new char[10]; // something's wrong if 10*10 digits of semaphores
   sprintf(semName, "%d", (int) systemSemaphores->first);

   // insert new semaphore
   (*(systemSemaphores->second))[systemSemaphores->first] = 
      new Semaphore(semName, initialValue);

   delete [] semName;

   // return semaphore ID
   machine->WriteRegister(2, systemSemaphores->first);

   // increase amount of semaphores
   (systemSemaphores->first)++;

   returnFromSystemCall();
}


/*
 *  System call interface: int SemDestroy( Sem_t )
 */
void NachOS_SemDestroy() {		// System call 12
   int semaphoreId = machine->ReadRegister(4);

   // fetch semaphore
   Semaphore* semaphoreToDestroy =
      (*(systemSemaphores->second))[semaphoreId];

   // destroy safely the semaphore
   semaphoreToDestroy->Destroy();

   // delete semaphore structure
   delete semaphoreToDestroy;

   // eliminate semaphore from tree
   size_t result = systemSemaphores->second->erase(semaphoreId);

   // returns amount of destroyed elements, as such failure or success
   machine->WriteRegister(2, result);

   // reduce amount of semaphores
   systemSemaphores->first--;

   returnFromSystemCall();
}


/*
 *  System call interface: int SemSignal( Sem_t )
 */
void NachOS_SemSignal() {		// System call 13
   int semaphoreId = machine->ReadRegister(4);

   int result = 1;

   // if semaphore does not exist
   if ((*systemSemaphores->second)[semaphoreId] == nullptr) {
      // report failure
      result = 0;
   } else { // otherwise
      // signal
      (*systemSemaphores->second)[semaphoreId]->V();
   }

   machine->WriteRegister(2, result);

   returnFromSystemCall();
}


/*
 *  System call interface: int SemWait( Sem_t )
 */
void NachOS_SemWait() {		// System call 14
   int semaphoreId = machine->ReadRegister(4);

   int result = 1;

   // if semaphore does not exist
   if ((*systemSemaphores->second)[semaphoreId] == nullptr) {
      // report failure
      result = 0;
   } else { // otherwise
      // wait
      (*systemSemaphores->second)[semaphoreId]->P();
   }

   machine->WriteRegister(2, result);

   returnFromSystemCall();
}


/*
 *  System call interface: Lock_t LockCreate( int )
 */
void NachOS_LockCreate() {		// System call 15
}


/*
 *  System call interface: int LockDestroy( Lock_t )
 */
void NachOS_LockDestroy() {		// System call 16
}


/*
 *  System call interface: int LockAcquire( Lock_t )
 */
void NachOS_LockAcquire() {		// System call 17
}


/*
 *  System call interface: int LockRelease( Lock_t )
 */
void NachOS_LockRelease() {		// System call 18
}


/*
 *  System call interface: Cond_t LockCreate( int )
 */
void NachOS_CondCreate() {		// System call 19
}


/*
 *  System call interface: int CondDestroy( Cond_t )
 */
void NachOS_CondDestroy() {		// System call 20
}


/*
 *  System call interface: int CondSignal( Cond_t )
 */
void NachOS_CondSignal() {		// System call 21
}


/*
 *  System call interface: int CondWait( Cond_t )
 */
void NachOS_CondWait() {		// System call 22
}


/*
 *  System call interface: int CondBroadcast( Cond_t )
 */
void NachOS_CondBroadcast() {		// System call 23
}


/*
 *  System call interface: Socket_t Socket( int, int )
 */
void NachOS_Socket() {			// System call 30
   int ipV = machine->ReadRegister(4);
   int socketType = machine->ReadRegister(5);
   int sslBuffer = machine->ReadRegister(6);

   bool ssl = (bool) sslBuffer;

   bool ipV6 = false;

   switch (ipV){
   case AF_INET_NachOS:
      ipV = AF_INET;
      break;
   case AF_INET6_NachOS:
      ipV = AF_INET6;
      ipV6 = true;
      break;
   default:
      break;
   }

   switch (socketType) {
      case SOCK_STREAM_NachOS:
         socketType = SOCK_STREAM;
         break;
      case SOCK_DGRAM_NachOS:
         socketType = SOCK_DGRAM;
         break;
      default:
         break;
   }

   int idSocket = socket(ipV, socketType, 0);

   int fileDescriptor = openFilesTable->Open(idSocket);

   machine->WriteRegister(2, fileDescriptor);

   if (idSocket != -1) {
      SocketInfo* newSocket = new SocketInfo(ipV6);

      systemSockets->insert({fileDescriptor, newSocket});

      (*systemSockets)[fileDescriptor]->SSLChoice = ssl;

      if (ssl) {
         SSL_CTX* SSLContext = nullptr;

         SSL* SSlStruct = InitSSL(SSLContext);

         (*systemSockets)[idSocket]->SSLContext = SSLContext;
         (*systemSockets)[idSocket]->SSLStruct = SSlStruct;
      }

   }

   returnFromSystemCall();
}


/*
 *  System call interface: Socket_t Connect(Socket_t, char * , int)
 */
void NachOS_Connect() {		// System call 31
   int idSocket = machine->ReadRegister(4);
   int ipMemLocation = machine->ReadRegister(5);
   int port = machine->ReadRegister(6);

   char* ip = new char[40];

   char current = 1;
   int idSocketBuffer = idSocket;
   int charPos = 0;
   int position = 0;

   // copy ip from memory into local variable
   while (current != 0) {
      charPos = position;
      machine->ReadMem(ipMemLocation + position, 1, (int*) &current);
      position = charPos;
      if (current != 0) {
         ip[position] = current;
      }
      ++position;
   };

   idSocket = idSocketBuffer;

   int fileDescriptor = openFilesTable->getUnixHandle(idSocket);

   // if SSL
   if ((*systemSockets)[fileDescriptor]->SSLChoice) {
      SSLConnect(
         (*systemSockets)[fileDescriptor]->ipV6,
         ip,
         port,
         fileDescriptor,
         (*systemSockets)[fileDescriptor]->SSLStruct);
   } else {
      socketConnect(
         (*systemSockets)[fileDescriptor]->ipV6,
         ip,
         port,
         fileDescriptor);
   }

   delete [] ip;

   returnFromSystemCall();
}


/*
 *  System call interface: int Bind( Socket_t, int )
 */
void NachOS_Bind() {		// System call 32
   int idSocket = machine->ReadRegister(4);
   int port = machine->ReadRegister(5);

   (void) idSocket;
   (void) port;

   returnFromSystemCall();
}


/*
 *  System call interface: int Listen( Socket_t, int )
 */
void NachOS_Listen() {		// System call 33

   returnFromSystemCall();
}


/*
 *  System call interface: int Accept( Socket_t )
 */
void NachOS_Accept() {		// System call 34

   returnFromSystemCall();
}


/*
 *  System call interface: int Shutdown( Socket_t, int )
 */
void NachOS_Shutdown() {	// System call 35

}

void nachOS_ProcessRequest()
{
    int requestType = machine->ReadRegister(4);

    std::string response;
    int totalAmount = 0;
    char buffer[501];
    std::string lastLine = "";
    std::string line = "";
    std::string endOfDoc = "";
    int cyclesSinceEndOfBytes = 4;
    int count = 0;

    if (requestType == 2)
    {
        printf("%-15s%s\n", "Cantidad", "Descripción");
        printf("%-15s%s\n", "--------", "-----------");
    }

    OpenFileId fileId = Open("output.txt");
    if (fileId < 0)
    {
        printf("Error opening output file\n");
        returnFromSystemCall();
        return;
    }

    int bytesRead;
    while ((bytesRead = Read(buffer, 500, ConsoleInput)) > 0)
    {
        buffer[bytesRead] = '\0';
        response = buffer;

        size_t character = 0;
        size_t initLocation = 0;
        bool endOfBytes = false;

        while (character < response.size())
        {
            endOfBytes = (character == response.size() - 1);

            if (response[character] == '\n' || endOfBytes)
            {
                if (cyclesSinceEndOfBytes == 2)
                {
                    endOfDoc += lastLine;
                    lastLine = endOfDoc;
                    size_t emptyTries = 0;
                    while (endOfDoc.empty() && emptyTries < 10)
                    {
                        emptyTries++;
                        bytesRead = Read(buffer, 500, ConsoleInput);
                        if (bytesRead <= 0)
                            break;
                        buffer[bytesRead] = '\0';
                        endOfDoc = buffer;
                    }
                    cyclesSinceEndOfBytes = 4;
                }

                int adjustment = 1;
                if (static_cast<unsigned int>(response[character - 1]) > 127)
                {
                    adjustment -= 2;
                }

                if (endOfBytes)
                {
                    endOfDoc = lastLine + response.substr(initLocation, character - initLocation + adjustment);
                    cyclesSinceEndOfBytes = 0;
                }

                line = lastLine + response.substr(initLocation, character - initLocation + adjustment);

                // Realizar análisis de expresión regular
                regexAnalyzer(requestType, line, totalAmount);

                lastLine = response.substr(initLocation, character - initLocation + adjustment);
                initLocation = character + 1;
                cyclesSinceEndOfBytes++;

                // Escribir la línea en el archivo
                char* lineBuffer = const_cast<char*>(line.c_str());
                Write(lineBuffer, line.size(), fileId);
            }
            character++;
        }

        if (requestType == 3 && response.size() < 500)
        {
            break;
        }
    }

    // Escribir el valor ASCII 4 al final del documento
    char eofMarker = 4;
    Write(&eofMarker, 1, fileId);

    Close(fileId);

    // Retornar el fileId del archivo
    machine->WriteRegister(2, fileId);

    if (requestType == 0)
    {
        // Salir del programa
        interrupt->Halt();
    }

    returnFromSystemCall();
}





void regexAnalyzer(int requestType, std::string &line, int &totalAmount)
{
    std::vector<std::string> figuresArray;
    std::vector<std::pair<std::string, int>> requestedPieces;

    if (requestType == 2)
    {
        // Regex to match menu options
        std::regex regexMenu("<OPTION\\s+value=\"(?!None\")([^\"]+)\">");
        std::smatch optionMatch;
        std::string::const_iterator begin(line.cbegin());
        if (std::regex_search(begin, line.cend(), optionMatch, regexMenu))
        {
            std::string figure = optionMatch[1];
            bool foundFigure = false;
            for (std::string figure : figuresArray)
            {
                if (figure == figure)
                {
                    foundFigure = true;
                    break;
                }
            }
            if (!foundFigure)
            {
                figuresArray.push_back(figure);
            }
            // Update position in the response string
            begin = optionMatch.suffix().first;
        }
    }
    else
    {
        // Regex to match pieces in figure response
        std::regex regexPiece("<TR><TD ALIGN=center?> (\\d+)</TD>\\s*<TD ALIGN=center> ([^<]+)</TD>");
        std::smatch pieceMatch;
        std::string::const_iterator begin(line.cbegin());

        if (std::regex_search(begin, line.cend(), pieceMatch, regexPiece))
        {
            // Extract quantity and description of the piece
            std::string amount = pieceMatch[1];
            std::string description = pieceMatch[2];

            bool foundPiece = false;
            for (auto &piece : requestedPieces)
            {
                if (piece.first == description)
                {
                    foundPiece = true;
                    break;
                }
            }

            if (!foundPiece)
            {
                // Convert quantity to integer
                int quantity = std::stoi(amount);
                printf("%-15s%s\n", std::to_string(quantity).c_str(), description.c_str());
                requestedPieces.emplace_back(description, quantity);
                totalAmount += quantity;
            }

            // Update position in the response string
            begin = pieceMatch.suffix().first;
        }
    }
}

void NachOS_InputRequest() {    // System call 37
   // get minRange, maxRange, and exception character from registers or memory
   int minRange = machine->ReadRegister(4);
   int maxRange = machine->ReadRegister(5);
   char exception;
   machine->ReadMem(machine->ReadRegister(6), 1, (int*) &exception);

   std::string inputString;

   // while string is readable, read
   while (std::cin >> inputString) {
      bool nonValidString = false;

      // Check if exception character is present and return it directly
      if (exception != '\0' && inputString[0] == exception && inputString.size() == 1) {
         machine->WriteRegister(2, (int) inputString[0]);
         returnFromSystemCall();
         return;
      }

      // for all characters read in the string
      for (size_t character = 0; character < inputString.size(); character++) {
         // check if value is numeric
         if (('0' > inputString[character] || inputString[character] > '9') &&
             (character == 0 && inputString[character] != '-')) {
            nonValidString = true;
         }
      }

      // If the input string is not a valid number, prompt the user to enter a valid number
      if (nonValidString) {
         // if not, prompt correct value
         std::cout << "\nMensaje: \"" << inputString
                   << "\" no es una opcion valida. Por favor introducir un numero entre\n"
                   << "[" << minRange << " y " << maxRange << "]" << std::endl;
         continue;
      }

      // change into a value
      int inputValue = atoi(inputString.c_str());

      // if within range
      if (minRange <= inputValue && inputValue <= maxRange) {
         machine->WriteRegister(2, inputValue);
         returnFromSystemCall();
         return;
      }

      std::cout << "\nMensaje: " << inputValue
                << " no esta dentro del rango, por favor introducir un valor entre\n"
                << "[" << minRange << " y " << maxRange << "]" << std::endl;
   }

   // if input was not readable, return error
   machine->WriteRegister(2, -2);
   returnFromSystemCall();
}


//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch ( which ) {

       case SyscallException:
          switch ( type ) {
             case SC_Halt:		// System call # 0
                NachOS_Halt();
                break;
             case SC_Exit:		// System call # 1
                NachOS_Exit();
                break;
             case SC_Exec:		// System call # 2
                NachOS_Exec();
                break;
             case SC_Join:		// System call # 3
                NachOS_Join();
                break;

             case SC_Create:		// System call # 4
                NachOS_Create();
                break;
             case SC_Open:		// System call # 5
                NachOS_Open();
                break;
             case SC_Read:		// System call # 6
                NachOS_Read();
                break;
             case SC_Write:		// System call # 7
                NachOS_Write();
                break;
             case SC_Close:		// System call # 8
                NachOS_Close();
                break;

             case SC_Fork:		// System call # 9
                NachOS_Fork();
                break;
             case SC_Yield:		// System call # 10
                NachOS_Yield();
                break;

             case SC_SemCreate:         // System call # 11
                NachOS_SemCreate();
                break;
             case SC_SemDestroy:        // System call # 12
                NachOS_SemDestroy();
                break;
             case SC_SemSignal:         // System call # 13
                NachOS_SemSignal();
                break;
             case SC_SemWait:           // System call # 14
                NachOS_SemWait();
                break;

             case SC_LckCreate:         // System call # 15
                NachOS_LockCreate();
                break;
             case SC_LckDestroy:        // System call # 16
                NachOS_LockDestroy();
                break;
             case SC_LckAcquire:         // System call # 17
                NachOS_LockAcquire();
                break;
             case SC_LckRelease:           // System call # 18
                NachOS_LockRelease();
                break;

             case SC_CondCreate:         // System call # 19
                NachOS_CondCreate();
                break;
             case SC_CondDestroy:        // System call # 20
                NachOS_CondDestroy();
                break;
             case SC_CondSignal:         // System call # 21
                NachOS_CondSignal();
                break;
             case SC_CondWait:           // System call # 22
                NachOS_CondWait();
                break;
             case SC_CondBroadcast:           // System call # 23
                NachOS_CondBroadcast();
                break;

             case SC_Socket:	// System call # 30
		NachOS_Socket();
               break;
             case SC_Connect:	// System call # 31
		NachOS_Connect();
               break;
             case SC_Bind:	// System call # 32
		NachOS_Bind();
               break;
             case SC_Listen:	// System call # 33
		NachOS_Listen();
               break;
             case SC_Accept:	// System call # 34
		NachOS_Accept();
               break;
             case SC_Shutdown:	// System call # 35
		NachOS_Shutdown();
               break;
             case SC_ProcessRequest:	// System call # 36
		nachOS_ProcessRequest();
               break;
             case SC_InputRequest:	// System call # 37
		NachOS_InputRequest();
               break;               
             default:
                printf("Unexpected syscall exception %d\n", type );
                ASSERT( false );
                break;
          }
          break;

       case PageFaultException: {
          break;
       }

       case ReadOnlyException:
          printf( "Read Only exception (%d)\n", which );
          ASSERT( false );
          break;

       case BusErrorException:
          printf( "Bus error exception (%d)\n", which );
          ASSERT( false );
          break;

       case AddressErrorException:
          printf( "Address error exception (%d)\n", which );
          ASSERT( false );
          break;

       case OverflowException:
          printf( "Overflow exception (%d)\n", which );
          ASSERT( false );
          break;

       case IllegalInstrException:
          printf( "Ilegal instruction exception (%d)\n", which );
          ASSERT( false );
          break;

       default:
          printf( "Unexpected exception %d\n", which );
          ASSERT( false );
          break;
    }

}
