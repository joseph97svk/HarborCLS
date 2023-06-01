#include "bitmap.h"

#include "NachosOpenFilesTable.h"

#include <iostream>

int MAX_FILES_OPEN = 100;

NachosOpenFilesTable::NachosOpenFilesTable() {
    this->openFilesMap = new BitMap(MAX_FILES_OPEN);
    this->openFiles = new int[MAX_FILES_OPEN];

    // set all open files value at -1
    memset(this->openFiles, -1, MAX_FILES_OPEN);

    // set input and output file descriptors as reserved
    this->openFilesMap->Mark(0);
    this->openFilesMap->Mark(1);
    this->openFilesMap->Mark(2);
    this->openFiles[0] = 0;
    this->openFiles[1] = 1;
    this->openFiles[2] = 2;

    usage = 1;
}

NachosOpenFilesTable::~NachosOpenFilesTable() {
    delete this->openFilesMap;
    delete [] this->openFiles;
}

int NachosOpenFilesTable::Open( int UnixHandle ) {
    int handle = 0;

    // for all positions on the openFiles array
    for (int openFilesIndex = 0;
        openFilesIndex < MAX_FILES_OPEN;
        openFilesIndex++) {

        // if position not occupied
        if (!this->openFilesMap->Test(openFilesIndex)) {
            // check next position
            continue;
        }

        // if position marked and has handle
        if (this->openFiles[openFilesIndex] == UnixHandle) {
            // NachOs handle is the current position
            handle = openFilesIndex;
            break;
        }
    }

    // if handle found and thus file already open
    if (handle != 0) {
        // report the handle
        return handle;
    }

    // find the handle if file not previously opened
    handle = this->openFilesMap->Find();

    // if no space found
    if (handle == -1) {
        // return error
        return -1;
    }

    // set the unix handle at the position found
    this->openFiles[handle] = UnixHandle;

    // return the NachOs handle
    return handle;
}	

int NachosOpenFilesTable::Close( int NachosHandle ) {
    // if not opened or impossible handle
    if (!isOpened(NachosHandle) ||
        NachosHandle < 0 || NachosHandle > MAX_FILES_OPEN) {
        // report failure as not opened
        return -1;
    }

    int UnixHandle = this->openFiles[NachosHandle]; // buffer the unix handle
    this->openFilesMap->Clear(NachosHandle); // clear on bitmap
    this->openFiles[NachosHandle] = -1; // reset Unix Handle to not open status
    // return the closed unix handle for closed document
    return UnixHandle;
}	

bool NachosOpenFilesTable::isOpened( int NachosHandle ) {
    // if impossible handle
    if (NachosHandle < 0 || NachosHandle > MAX_FILES_OPEN) {
        // report error
        return false;
    }

    return this->openFilesMap->Test(NachosHandle);
}

int NachosOpenFilesTable::getUnixHandle( int NachosHandle ) {
    // if impossible handle
    if (NachosHandle < 0 || NachosHandle > MAX_FILES_OPEN) {
        // report error
        return -1;
    }
    
    // find UnixHandle
    // if not opened, will return error message -1
    return this->openFiles[NachosHandle];
}

void NachosOpenFilesTable::addThread() {
    this->usage++;
}	
int NachosOpenFilesTable::delThread() {
    this->usage--;
    return this->usage;
}						

void NachosOpenFilesTable::Print() {
    // for all possible files
    for (int file = 0; file < MAX_FILES_OPEN; file++) {
        // if current file position is open
        if (this->isOpened(file)) {
            // print the NachOs handle and Unix Handle
            printf("%i, %i\n", file, this->openFiles[file]);
        }
    }
}							