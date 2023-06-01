// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "../bin/noff.h"
#include <iostream>

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    this->openFilesTable = new NachosOpenFilesTable();

    NoffHeader noffH;
    unsigned int i, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    // how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);

    // find if there are enough space to fit program
    if (memMap->NumClear() < (int) numPages) {
        int memLeft = memMap->NumClear();
        // report error
        fprintf(stderr, "Could not allocate necessary memory, terminating thread\n"
        "Amount of memory left: %i out of %i.\nWanted: %i\n", memLeft, NumPhysPages, (int)numPages);
        // terminate thread
        currentThread->Finish();
        // handle lack of space
    }

    unsigned int sizeToCopy = noffH.code.size + noffH.initData.size + noffH.uninitData.size;
    sizeToCopy = divRoundUp(size, PageSize);

    /*
    memMap->Mark(2);
    memMap->Mark(4);
    memMap->Mark(8);
    memMap->Mark(10);
    memMap->Mark(12);
    */

    size = numPages * PageSize;

    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);

    int pageLocation = 0;

    // first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        // find empty page
        // does not matter if pages are not contiguous
        // expected for address space to handle correctly by use of virtual address
        int newLocation = memMap->Find();
        // if page invalid
        if (newLocation == -1) {
            // for all pages marked
            int memLeft = memMap->NumClear();
            for (unsigned int pageToErase = 0;
                pageToErase < i;
                pageToErase++) {
                // clear them
                memMap->Clear(this->pageTable[pageToErase].physicalPage); 
            }
            // report error
            fprintf(stderr, "Could not allocate necessary memory, terminating thread\n"
            "Amount of memory left: %i out of %i\n", memLeft, NumPhysPages);
            // terminate thread
            currentThread->Finish();
        }

        pageLocation = newLocation;

        pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
        pageTable[i].physicalPage = pageLocation; // the location is the position within the bitmap
        pageTable[i].valid = true;
        pageTable[i].use = false;
        pageTable[i].dirty = false;
        pageTable[i].readOnly = false;  // if the code segment was entirely on 
                        // a separate page, we could set its 
                        // pages to be read-only
    }

    // for all pages
    for (unsigned int page = 0; page < sizeToCopy ; page++) {
        // find the page location
        int newPageLocation = this->pageTable[page].physicalPage;
        int positionOnFile = noffH.code.inFileAddr // begin of file location
            + (page // position of page after file start
            * PageSize); // with a page size displacement

        DEBUG('a', "Copying page at page %d,"
            " and file location 0x%x\n", 
			newPageLocation, positionOnFile);

        int pageCopySize = PageSize;

        // if last page
        if (page == sizeToCopy - 1) {
            // copy just the bytes in last page that are within file
            pageCopySize =
                (noffH.code.size + noffH.initData.size + noffH.uninitData.size)
                % sizeToCopy; 
        }

        // copy entire page into memory
        executable->ReadAt(
            &(machine->mainMemory[PageSize * newPageLocation]),  // at location of page within memory
            pageCopySize, // with the size of a page
            positionOnFile);
    }
}


AddrSpace::AddrSpace(AddrSpace *other) {
    this->numPages = other->numPages;
    this->openFilesTable = other->openFilesTable;
    this->forked = true;

    unsigned int pageLocation = 0;
    // size shared is the all sectors minus the stack
    unsigned int sharedMemory = this->numPages
        - divRoundUp(UserStackSize, PageSize);

    // first, set up the translation 
    pageTable = new TranslationEntry[this->numPages];
    unsigned int page = 0;

    // set as shared the code and data segments
    for (; page < sharedMemory; page++) {
        // set physical location as same for the father
        pageLocation = other->pageTable[page].physicalPage;

        pageTable[page].virtualPage = page;	// for now, virtual page # = phys page #
        pageTable[page].physicalPage = pageLocation; // the location is the position within the bitmap
        pageTable[page].valid = true;
        pageTable[page].use = false;
        pageTable[page].dirty = false;
        pageTable[page].readOnly = false;  // if the code segment was entirely on 
                        // a separate page, we could set its 
                        // pages to be read-only
    }

    // allocate new space for the stack
    for (; page < this->numPages; page++) {
        // find empty page
        // does not matter if pages are not contiguous
        // expected for address space to handle correctly by use of virtual address
        int newLocation = memMap->Find();

        // if page invalid
        if (newLocation == -1) {
            int memLeft = memMap->NumClear();
            // for all pages marked
            for (unsigned int pageToErase = sharedMemory;
                pageToErase < page;
                pageToErase++) {
                // clear them
                memMap->Clear(this->pageTable[pageToErase].physicalPage); 
            }
            // report error
            // report error
            fprintf(stderr, "Could not allocate necessary memory, terminating thread\n"
            "Amount of memory used: %i out of %i\n", memLeft, NumPhysPages);
            // terminate thread
            currentThread->Finish();
        }

        pageLocation = newLocation;

        pageTable[page].virtualPage = page;	// for now, virtual page # = phys page #
        pageTable[page].physicalPage = pageLocation; // the location is the position within the bitmap
        pageTable[page].valid = true;
        pageTable[page].use = false;
        pageTable[page].dirty = false;
        pageTable[page].readOnly = false;  // if the code segment was entirely on 
                        // a separate page, we could set its 
                        // pages to be read-only
    }
}


//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------
AddrSpace::~AddrSpace()
{
    if (this->forked) {
        unsigned int sharedMemory = this->numPages
            - divRoundUp(UserStackSize, PageSize);
        for (unsigned int page = sharedMemory;
            page < this->numPages;
            page++
            ) {
            memMap->Clear(this->pageTable[page].physicalPage);
        }
    } else {
        // clear used pages
        for (unsigned int page = 0; page < this->numPages; page++) {
            memMap->Clear(this->pageTable[page].physicalPage);
        }
    }

    delete pageTable;

    // if I was the last thread using it
    if (this->openFilesTable->delThread() == 0) {
        // destroy openFilesTable
        delete this->openFilesTable;
    }
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
