#include <proc.h>
#include <stdio.h>
#include <syscall.h>
#include <x86.h>

int main(int argc, char **argv)
{
#if LAB <= 5
    printf("idle\n");

    pid_t ping_pid, pong_pid, ding_pid;

    if ((ping_pid = spawn(1, 1000)) != -1)
        printf("ping in process %d.\n", ping_pid);
    else
        printf("Failed to launch ping.\n");

    if ((pong_pid = spawn(2, 1000)) != -1)
        printf("pong in process %d.\n", pong_pid);
    else
        printf("Failed to launch pong.\n");

    if ((ding_pid = spawn(3, 1000)) != -1)
        printf("ding in process %d.\n", ding_pid);
    else
        printf("Failed to launch ding.\n");

    /*
     * Lab 5 (optional): once sys_fork works, launch the fork test with
     *     spawn(5, 1000);
     */
#elif LAB == 6
    /* Lab 6: the kernel itself creates ping and pong. */
    while (1) {}
#elif LAB == 8
    /*
     * Course project: spawn the demonstration program(s) of your project
     * here, e.g. spawn(<elf_id>, 1000). See the project description for how
     * to add a new user program and its elf_id.
     */
    printf("idle: project mode (LAB=8)\n");
    while (1)
        yield();
#else
    printf("idle\n");

    pid_t fstest_pid;

    if ((fstest_pid = spawn(4, 1000)) != -1)
        printf("fstest in process %d.\n", fstest_pid);
    else
        printf("Failed to launch fstest.\n");
#endif

    return 0;
}
