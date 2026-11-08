#include <proc.h>
#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
#if LAB <= 5
    /* Lab 5: each process has its own address space. */
    unsigned int val = 300;
    unsigned int *addr = (unsigned int *) 0xe0000000;

    printf("ding started.\n");
    printf("ding: the value at address %x: %d\n", addr, *addr);
    printf("ding: writing the value %d to the address %x\n", val, addr);
    *addr = val;
    yield();
    printf("ding: the new value at address %x: %d\n", addr, *addr);
#elif LAB == 6
    /* Lab 6: ping produces and pong consumes, on different CPUs. */
    printf("ding started.\n");
#else
    printf("ding started.\n");
#endif

    return 0;
}
