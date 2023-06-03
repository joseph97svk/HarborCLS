/* syscalls.h 
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel. 
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation 
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */
#define SC_Halt		0
#define SC_Exit		1
#define SC_Exec		2
#define SC_Join		3
#define SC_Create	4
#define SC_Open		5
#define SC_Read		6
#define SC_Write	7
#define SC_Close	8
#define SC_Fork		9
#define SC_Yield	10
/*
 *  Synchronization system calls
 */
#define SC_SemCreate	11
#define SC_SemDestroy	12
#define SC_SemSignal	13
#define SC_SemWait	14
#define SC_LckCreate	15
#define SC_LckDestroy	16
#define SC_LckAcquire	17
#define SC_LckRelease	18
#define SC_CondCreate	19
#define SC_CondDestroy	20
#define SC_CondSignal	21
#define SC_CondWait	22
#define SC_CondBroadcast	23

/*
 *  Socket system calls
 */
#define SC_Socket	30
#define SC_Connect	31
#define SC_Bind		32
#define SC_Listen	33
#define SC_Accept	34
#define SC_Shutdown	35


// for read and write system calls
#define CONSOLE_INPUT   0
#define CONSOLE_OUTPUT  1

#ifndef IN_ASM

/* The system call interface.  These are the operations the Nachos
 * kernel needs to support, to be able to run user programs.
 *
 * Each of these is invoked by a user program by simply calling the 
 * procedure; an assembly language stub stuffs the system call code
 * into a register, and traps to the kernel.  The kernel procedures
 * are then invoked in the Nachos kernel, after appropriate error checking, 
 * from the system call entry point in exception.cc.
 */

/* Stop Nachos, and print out performance stats */
void Halt();		
 

/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit(int status);	

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;	
 
/* Run the executable, stored in the Nachos file "name", and return the 
 * address space identifier
 */
SpaceId Exec(char *name);
 
/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */
int Join(SpaceId id); 	
 

/* File system operations: Create, Open, Read, Write, Close
 * These functions are patterned after UNIX -- files represent
 * both files *and* hardware I/O devices.
 *
 * If this assignment is done before doing the file system assignment,
 * note that the Nachos file system has a stub implementation, which
 * will work for the purposes of testing out these routines.
 */
 
/* A unique identifier for an open Nachos file. */
typedef int OpenFileId;	

/* when an address space starts up, it has two open files, representing 
 * keyboard input and display output (in UNIX terms, stdin and stdout).
 * Read and Write can be used directly on these, without first opening
 * the console device.
 */

#define ConsoleInput	0  
#define ConsoleOutput	1  
#define ConsoleError	2  
 
/* Create a Nachos file, with "name" */
void Create(char *name);

/* Open the Nachos file "name", and return an "OpenFileId" that can 
 * be used to read and write to the file.
 */
OpenFileId Open(char *name);

/* Write "size" bytes from "buffer" to the open file. */
void Write(char *buffer, int image.png, OpenFileId id);

/* Read "size" bytes from the open file into "buffer".  
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough 
 * characters to read, return whatever is available (for I/O devices, 
 * you should always wait until you can return at least one character).
 */
int Read(char *buffer, int size, OpenFileId id);

/* Close the file, we're done reading and writing to it. */
void Close(OpenFileId id);



/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program. 
 */

/* Fork a thread to run a procedure ("func") in the *same* address space 
 * as the current thread.
 */
void Fork(void (*func)());

/* Yield the CPU to another runnable thread, whether in this address space 
 * or not. 
 */
void Yield();		


typedef int Sem_t;
/* SemCreate creates a semaphore initialized to initval value
 * return the semaphore id
 */
Sem_t SemCreate( int initVal );

/* SemDestroy destroys a semaphore identified by semId */ 
int SemDestroy( Sem_t semId );

/* SemSignal signals a semaphore, awakening some other thread if necessary */
int SemSignal( Sem_t semId );

/* SemWait waits a semaphore, some other thread may awake if one blocked */
int SemWait( Sem_t semId );

typedef int Lock_t;
/* LckCreate creates a lock structure */
Lock_t LckCreate( );

/* LckDestroy destroy an already created Lock */
int LckDestroy( Lock_t lockId );

/* LckLock obtain the lock, if busy the thread must wait */
int LckLock( Lock_t lockId );

/* LckUnlock release the obtained lock freeing it to be used for others */
int LckUnlock( Lock_t lockId );

typedef int Cond_t;
/* CondCreate creates a condition variable */
int CondCreate( Cond_t condId );

/* CondDestroy destroy an already created Lock */
int LckDestroy( Cond_t condId );

/* CondSignal signal on condition variable, awaking other threads if necessary */
int CondSignal( Cond_t condId, Lock_t lockId );

/* CondWait wait for the condition to meet, thread must wait */
int CondWait( Cond_t condId, Lock_t lockId );

/* CondBroadcast awakes all waiting threads on this variable */
int CondBroadcast( Cond_t condId, Lock_t lockId );

/*
 *  NachOS sockets system call family
 */
typedef int Socket_t;
#define AF_INET_NachOS		0
#define AF_INET6_NachOS		1
#define SOCK_STREAM_NachOS	0
#define SOCK_DGRAM_NachOS	1

/* Socket creates a Unix socket
 *  socket family: 0 for AF_INET, 1 for AF_INET6
 *  socket type: 0 for SOCK_STREAM, 1 for SOCK_DGRAM
 *  socket protocol: 0
 *
 */
Socket_t Socket( int family, int type, int SSL);

/* Connects a cliet to a listen server */
int Connect( Socket_t SockId, char * IP_Addr, int port);

/*  Binds a port to the calling thread */
int Bind( Socket_t SockId, int port );

/* Listens for connections and define a backlog queue */
int Listen( Socket_t SockId, int backlog );

/* Accepts connections from clients and returns a new socket descriptor */
int Accept( Socket_t SockId );

/* Shutdowns a socket connection, mode indicates Read, Write, ReadWrite */
int Shutdown( Socket_t SockId, int mode );

#endif /* IN_ASM */

#endif /* SYSCALL_H */
