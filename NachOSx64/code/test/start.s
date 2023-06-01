/* Start.s 
 *	Assembly language assist for user programs running on top of Nachos.
 *
 *	Since we don't want to pull in the entire C library, we define
 *	what we need for a user program here, namely Start and the system
 *	calls.
 */

#define IN_ASM
#include "syscall.h"

        .text   
        .align  2

/* -------------------------------------------------------------
 * __start
 *	Initialize running a C program, by calling "main". 
 *
 * 	NOTE: This has to be first, so that it gets loaded at location 0.
 *	The Nachos kernel always starts a program by jumping to location 0.
 * -------------------------------------------------------------
 */

	.globl __start
	.ent	__start
__start:
	jal	main
	move	$4,$0		
	jal	Exit	 /* if we return from main, exit(0) */
	.end __start

/* -------------------------------------------------------------
 * System call stubs:
 *	Assembly language assist to make system calls to the Nachos kernel.
 *	There is one stub per system call, that places the code for the
 *	system call into register r2, and leaves the arguments to the
 *	system call alone (in other words, arg1 is in r4, arg2 is 
 *	in r5, arg3 is in r6, arg4 is in r7)
 *
 * 	The return value is in r2. This follows the standard C calling
 * 	convention on the MIPS.
 * -------------------------------------------------------------
 */

	.globl Halt
	.ent	Halt
Halt:
	addiu $2,$0,SC_Halt 
	syscall
	j	$31
	.end Halt

	.globl Exit
	.ent	Exit
Exit:
	addiu $2,$0,SC_Exit
	syscall
	j	$31
	.end Exit

	.globl Exec
	.ent	Exec
Exec:
	addiu $2,$0,SC_Exec
	syscall
	j	$31
	.end Exec

	.globl Join
	.ent	Join
Join:
	addiu $2,$0,SC_Join
	syscall
	j	$31
	.end Join

	.globl Create
	.ent	Create
Create:
	addiu $2,$0,SC_Create
	syscall
	j	$31
	.end Create

	.globl Open
	.ent	Open
Open:
	addiu $2,$0,SC_Open
	syscall
	j	$31
	.end Open

	.globl Read
	.ent	Read
Read:
	addiu $2,$0,SC_Read
	syscall
	j	$31
	.end Read

	.globl Write
	.ent	Write
Write:
	addiu $2,$0,SC_Write
	syscall
	j	$31
	.end Write

	.globl Close
	.ent	Close
Close:
	addiu $2,$0,SC_Close
	syscall
	j	$31
	.end Close

	.globl Fork
	.ent	Fork
Fork:
	addiu $2,$0,SC_Fork
	syscall
	j	$31
	.end Fork

	.globl Yield
	.ent	Yield
Yield:
	addiu $2,$0,SC_Yield
	syscall
	j	$31
	.end Yield

	.globl SemCreate
	.ent	SemCreate
SemCreate:
	addiu $2,$0,SC_SemCreate
	syscall
	j	$31
	.end SemCreate

	.globl SemDestroy
	.ent	SemDestroy
SemDestroy:
	addiu $2,$0,SC_SemDestroy
	syscall
	j	$31
	.end SemDestroy

	.globl SemSignal
	.ent	SemSignal
SemSignal:
	addiu $2,$0,SC_SemSignal
	syscall
	j	$31
	.end SemSignal

	.globl SemWait
	.ent	SemWait
SemWait:
	addiu $2,$0,SC_SemWait
	syscall
	j	$31
	.end SemWait

	.globl LckCreate
	.ent	LckCreate
LckCreate:
	addiu $2,$0,SC_LckCreate
	syscall
	j	$31
	.end LckCreate

	.globl LckDestroy
	.ent	LckDestroy
LckDestroy:
	addiu $2,$0,SC_LckDestroy
	syscall
	j	$31
	.end LckDestroy

	.globl LckAcquire
	.ent	LckAcquire
LckAcquire:
	addiu $2,$0,SC_LckAcquire
	syscall
	j	$31
	.end LckAcquire

	.globl LckRelease
	.ent	LckRelease
LckRelease:
	addiu $2,$0,SC_LckRelease
	syscall
	j	$31
	.end LckRelease

	.globl CondCreate
	.ent	CondCreate
CondCreate:
	addiu $2,$0,SC_CondCreate
	syscall
	j	$31
	.end CondCreate

	.globl CondDestroy
	.ent	CondDestroy
CondDestroy:
	addiu $2,$0,SC_CondDestroy
	syscall
	j	$31
	.end CondDestroy

	.globl CondAcquire
	.ent	CondAcquire
CondSignal:
	addiu $2,$0,SC_CondSignal
	syscall
	j	$31
	.end CondAcquire

	.globl CondSignal
	.ent	CondSignal
CondWait:
	addiu $2,$0,SC_CondWait
	syscall
	j	$31
	.end CondWait

	.globl CondBroadcast
	.ent	CondBroadcast
CondBroadCast:
	addiu $2,$0,SC_CondBroadcast
	syscall
	j	$31
	.end CondBroadcast

	.globl Socket
	.ent	Socket
Socket:
	addiu $2,$0,SC_Socket
	syscall
	j	$31
	.end Socket

	.globl Connect
	.ent	Connect
Connect:
	addiu $2,$0,SC_Connect
	syscall
	j	$31
	.end Connect

	.globl Bind
	.ent	Bind
Bind:
	addiu $2,$0,SC_Bind
	syscall
	j	$31
	.end Bind

	.globl Listen
	.ent	Listen
Listen:
	addiu $2,$0,SC_Listen
	syscall
	j	$31
	.end Listen

	.globl Accept
	.ent	Accept
Accept:
	addiu $2,$0,SC_Accept
	syscall
	j	$31
	.end Accept

	.globl Shutdown
	.ent	Shutdown
Shutdown:
	addiu $2,$0,SC_Shutdown
	syscall
	j	$31
	.end Shutdown

/* dummy function to keep gcc happy */
        .globl  __main
        .ent    __main
__main:
        j       $31
        .end    __main

