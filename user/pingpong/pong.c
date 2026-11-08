#include <proc.h>
#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
#if LAB <= 5
    /* Lab 5: each process has its own address space. */
    unsigned int val = 200;
    unsigned int *addr = (unsigned int *) 0xe0000000;

    printf("pong started.\n");
    printf("pong: the value at address %x: %d\n", addr, *addr);
    printf("pong: writing the value %d to the address %x\n", val, addr);
    *addr = val;
    yield();
    printf("pong: the new value at address %x: %d\n", addr, *addr);
#elif LAB == 6
    /* Lab 6: ping produces and pong consumes, on different CPUs. */
    unsigned int i;
    printf("pong started.\n");

    for (i = 0; i < 20; i++) {
        if (i % 2 == 0)
            consume();
    }
#else
    printf("pong started.\n");
#endif

    return 0;
}
