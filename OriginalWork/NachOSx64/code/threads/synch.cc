// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <stdexcept>

#include "copyright.h"
#include "synch.h"
#include "system.h"

#include <iostream>

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = (char *)debugName;
    value = initialValue;
    queue = new List<Thread*>;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    while (value == 0) { 			// semaphore not available (ask)
        queue->Append(currentThread);		// so go to sleep
        currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    interrupt->SetLevel(oldLevel);		// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    interrupt->SetLevel(oldLevel);
}

#ifdef USER_PROGRAM
//----------------------------------------------------------------------
// Semaphore::Destroy
// 	Destroy the semaphore, freeing the waiting threads
//	This is used to destroy a user semaphore
//----------------------------------------------------------------------

void
Semaphore::Destroy()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    while ( (thread = queue->Remove() ) != NULL )	// make thread ready
	scheduler->ReadyToRun(thread);

    interrupt->SetLevel(oldLevel);
}

#endif


// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(const char* debugName) {
    this->name = (char *)debugName;
    this->state = FREE;
    this->ILockedIt = false;
    this->queue = new List<Thread*>();
}


Lock::~Lock() {
    delete this->queue;
}

void Lock::Acquire() {
    IntStatus oldLevel = interrupt->SetLevel(IntOff); // lock interruptions

    // wait until lock is free
    // while the state is busy
    while (this->state == BUSY) {
        // thread is thrown into queue 
        this->queue->Append(currentThread);
        // sleep
        currentThread->Sleep();
    }

    // set it busy
    this->state = BUSY;
    this->ILockedIt = true;

    interrupt->SetLevel(oldLevel); // unlock interruptions
}


void Lock::Release() {
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff); // lock interruptions

    // take thread out of queue
    thread = queue->Remove();
    // if thread is not null
    if (thread != NULL) {
        // wake up thread that is waiting
        scheduler->ReadyToRun(thread);
    }

    // if unlocked by thread that did not lock it
    if (!this->ILockedIt) {
        // report fatal error
        throw std::runtime_error("Lock released by thread that did not lock it");
    }

    // set lock to free
    this->state = FREE;
    this->ILockedIt = false;

    interrupt->SetLevel(oldLevel); // unlock interruptions
}

bool Lock::isHeldByCurrentThread() {
   return this->ILockedIt;
}

Condition::Condition(const char* debugName) {
    this->name = (char*) debugName;
    this->queue = new List<Thread*>();
    this->amount = 0;
    this->expectedAmount = 0;
}


Condition::~Condition() {
    delete this->queue;
}


void Condition::Wait( Lock * conditionLock ) {

    // release the lock
    conditionLock->Release();
    
    // lock interruptions
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    amount++;
    expectedAmount++;
 
    // relinquish the CPU until signaled

    // while not allowed to continue
    while(amount == expectedAmount) {
        // queue again
        this->queue->Append(currentThread);
        // sleep
        currentThread->Sleep();
    }

    this->amount--;

    // unlock interruptions
    interrupt->SetLevel(oldLevel); 

    // then re-acquire the lock
    conditionLock->Acquire();
}


void Condition::Signal(Lock * conditionLock) {
    Thread *thread;

    // hold the lock
    if (!conditionLock->isHeldByCurrentThread()) {
        conditionLock->Acquire();
    }

    // lock interruptions 
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    // wake up a thread, if there are any waiting on the condition
	// take thread out of queue
    thread = this->queue->Remove();

    // if thread is not null
    if (thread != NULL) {
        // wake up thread that is waiting
        scheduler->ReadyToRun(thread);

        this->expectedAmount--;
    }

    // unlock interruptions
    interrupt->SetLevel(oldLevel); 

    // release the lock
    if (!conditionLock->isHeldByCurrentThread()) {
        conditionLock->Release();
    }
}

void Condition::Broadcast( Lock * conditionLock ) {
    // wake up all threads waiting on the condition

    // while there are threads waiting
    while (!this->queue->IsEmpty()) {
        // wake em up motherfuckers
        this->Signal(conditionLock);
    }
}


// Mutex class
Mutex::Mutex( const char * debugName ) {

}

Mutex::~Mutex() {

}

void Mutex::Lock() {

}

void Mutex::Unlock() {

}


// Barrier class
Barrier::Barrier( const char * debugName, int count ) {
}

Barrier::~Barrier() {
}

void Barrier::Wait() {
}

