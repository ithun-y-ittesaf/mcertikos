#include <proc.h>
#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
#if LAB <= 5
    /* Lab 5: each process has its own address space. */
    unsigned int val = 100;
    unsigned int *addr = (unsigned int *) 0xe0000000;

    printf("ping started.\n");
    printf("ping: the value at address %x: %d\n", addr, *addr);
    printf("ping: writing the value %d to the address %x\n", val, addr);
    *addr = val;
    yield();
    printf("ping: the new value at address %x: %d\n", addr, *addr);
#elif LAB == 6
    /* Lab 6: ping produces and pong consumes, on different CPUs. */
    unsigned int i;
    printf("ping started.\n");

    // fast producing
    for (i = 0; i < 10; i++)
        produce();

    // slow producing
    for (i = 0; i < 40; i++) {
        if (i % 4 == 0)
            produce();
    }
#else
    printf("ping started.\n");
#endif

    return 0;
}
