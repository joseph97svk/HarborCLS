#include "syscall.h"

int sem;
int sem2;
int counter;

void subroutine();

int main() {
    Write("Entering my test\n", 17, 1);
    counter = 0;
    sem = SemCreate(0);
    sem2 = SemCreate(1);

    Fork(subroutine);

    Write("waiting\n", 8, 1);
    SemWait(sem);
    Write("Done waiting\n", 13, 1);
    SemSignal(sem2);
    //Write("Now testing memory leak prevention\n", 35, 1);

    int i = 0;
    int process = 0;
    while (i < 30) {
        Write("\nNext try\n", 40, 1);
        process = Exec("../test/addrspacetest");
        Join(process);
        i++;
    }

    Write("Success!!!\n", 11, 1);
}

void subroutine() {
    char buffer[30];
    while (counter < 5) {
        Write("Choose right number to test, between 0 and 9\n", 45, 1);
        Write("You've got 5 tries\n", 19, 1);
        int responseLength = 0;
        while (responseLength <= 1) {
            int i = 0;
            do {
                responseLength += Read(&buffer[i], 1, 0); 
            } while( buffer[i++] != '\n' );
        }

        Write(buffer, 2, 1);

        if (buffer[0] == '0') {
            SemSignal(sem);
            Write("signaled\n", 9, 1);
            unsigned int wait = 3000000/*4294967295*/;
            while (wait != 0) {
                wait--;
            }
            SemWait(sem2);
            Write("Exit subroutine\n", 16, 1);
            return;
        }
        counter++;
    }
    //
    SemSignal(sem);
    Write("failed\n", 9, 1);
}

