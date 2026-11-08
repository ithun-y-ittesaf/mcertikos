#include <lib/debug.h>
#include <lib/types.h>
#include <lib/kstack.h>
#include <lib/monitor.h>
#include <lib/thread.h>
#include <lib/x86.h>
#include <dev/devinit.h>
#include <pcpu/PCPUIntro/export.h>
#include <pmm/MATInit/export.h>
#include <pmm/MContainer/export.h>
#include <vmm/MPTInit/export.h>
#include <vmm/MPTKern/export.h>
#include <proc/PProc/export.h>
#include <thread/PCurID/export.h>
#include <thread/PKCtxIntro/export.h>
#include <thread/PTCBIntro/export.h>
#include <thread/PTQueueInit/export.h>
#include <thread/PThread/export.h>
#include <trap/TSyscall/bbuf.h>

/*
 * This single project carries all seven lab experiments. The LAB macro (set
 * with "make LAB=<n>", see the top-level Makefile) selects how far the kernel
 * boots and which tests are run:
 *
 *   LAB 1: physical memory allocator            (kern/pmm/MAT*)
 *   LAB 2: memory containers and page tables    (kern/pmm/MContainer, kern/vmm/MPTIntro)
 *   LAB 3: virtual memory management            (kern/vmm/MPTOp, MPTComm, MPTKern, MPTNew)
 *   LAB 4: kernel threads and scheduling        (kern/thread/*)
 *   LAB 5: processes, traps and system calls    (kern/trap/TSyscallArg, TSyscall, TTrapHandler)
 *   LAB 6: multiprocessor and synchronization   (kern/trap/TTrapInit, sleep/wakeup,
 *                                                bounded buffer, producer/consumer)
 *   LAB 7: disk and file system                 (disk interrupt, kern/fs/*)
 *   LAB 8: course project (boots like lab 7; user/idle/idle.c starts the demo)
 *
 * Search the code for "TODO [Lab <n>]" to find the tasks of lab <n>.
 */
#ifndef LAB
#define LAB 1
#endif

#ifdef TEST
extern bool test_MATIntro(void);
extern bool test_MATInit(void);
extern bool test_MATOp(void);
extern bool test_MContainer(void);
extern bool test_MPTIntro(void);
extern bool test_MPTOp(void);
extern bool test_MPTComm(void);
extern bool test_MPTKern(void);
extern bool test_MPTNew(void);
extern bool test_PKCtxNew(void);
extern bool test_PTCBInit(void);
extern bool test_PTQueueInit(void);
extern bool test_PThread(void);
extern bool test_TSyscallArg(void);
extern bool test_TSyscall(void);
extern bool test_TTrapInit(void);
extern bool test_PThreadSleep(void);
extern bool test_BBuffer(void);

static int tests_run, tests_passed;

static void run_test(const char *layer, bool (*test)(void))
{
    dprintf("Testing the %s layer...\n", layer);
    tests_run++;
    if (test() == 0) {
        tests_passed++;
        dprintf("All tests passed.\n");
    } else {
        dprintf("Test failed.\n");
    }
    dprintf("\n");
}

static void run_tests(void)
{
    tests_run = tests_passed = 0;

#if LAB == 1
    run_test("MATIntro", test_MATIntro);
    run_test("MATInit", test_MATInit);
    run_test("MATOp", test_MATOp);
#elif LAB == 2
    run_test("MContainer", test_MContainer);
    run_test("MPTIntro", test_MPTIntro);
#elif LAB == 3
    run_test("MPTOp", test_MPTOp);
    run_test("MPTComm", test_MPTComm);
    run_test("MPTKern", test_MPTKern);
    run_test("MPTNew", test_MPTNew);
#elif LAB == 4
    run_test("PKCtxNew", test_PKCtxNew);
    run_test("PTCBInit", test_PTCBInit);
    run_test("PTQueueInit", test_PTQueueInit);
    run_test("PThread", test_PThread);
#elif LAB == 5
    run_test("TSyscallArg", test_TSyscallArg);
    run_test("TSyscall (sys_spawn)", test_TSyscall);
#elif LAB == 6
    run_test("TTrapInit", test_TTrapInit);
    run_test("PThread (sleep/wakeup)", test_PThreadSleep);
    run_test("bounded buffer", test_BBuffer);
#endif

    dprintf("==================================================\n");
    dprintf("LAB %d RESULT: %d / %d test groups passed.\n", LAB, tests_passed, tests_run);
    dprintf("==================================================\n");
    dprintf("\nTest complete. Please Use Ctrl-a x to exit qemu.");
}
#endif

#if LAB <= 5

static void kern_main(void)
{
    KERN_DEBUG("In kernel main.\n\n");

#ifdef TEST
    run_tests();
#else
    /* From lab 5 on, type "startuser" in the monitor to start the user processes. */
    monitor(NULL);
#endif
}

void kern_init(uintptr_t mbi_addr)
{
#if LAB == 1
    pmem_init(mbi_addr);
#elif LAB == 2
    container_init(mbi_addr);
#elif LAB == 3
#ifdef TEST
    pdir_init_kern(mbi_addr);
#else
    paging_init(mbi_addr);
#endif
#else
    thread_init(mbi_addr);
#endif

    KERN_DEBUG("Kernel initialized.\n");

    kern_main();
}

void kern_init_ap(void (*f)(void))
{
    devinit_ap();
    f();
}

#elif LAB == 6

extern uint32_t pcpu_ncpu(void);
extern int pcpu_boot_ap(uint32_t cpu_idx, void (*f)(void), uintptr_t stack_addr);

static volatile int cpu_booted = 0;
static volatile int all_ready = FALSE;
static void kern_main_ap(void);

extern uint8_t _binary___obj_user_idle_idle_start[];
extern uint8_t _binary___obj_user_pingpong_ping_start[];
extern uint8_t _binary___obj_user_pingpong_pong_start[];

#ifndef TEST
/* Starts the first process in the ready queue of the current CPU. */
static void start_first_process(unsigned int pid)
{
    int cpu_idx = get_pcpu_idx();

    tqueue_remove(NUM_IDS + cpu_idx, pid);
    tcb_set_state(pid, TSTATE_RUN);
    set_curid(pid);
    kctx_switch(0, pid);

    KERN_PANIC("start_first_process() should never reach here.\n");
}
#endif

static void kern_main(void)
{
    KERN_INFO("[BSP KERN] In kernel main.\n\n");

    KERN_INFO("[BSP KERN] Number of CPUs in this system: %d. \n", pcpu_ncpu());

    int i;

    all_ready = FALSE;
    for (i = 1; i < pcpu_ncpu(); i++) {
        KERN_INFO("[BSP KERN] Boot CPU %d .... \n", i);

        bsp_kstack[i].cpu_idx = i;
        pcpu_boot_ap(i, kern_main_ap, (uintptr_t) &bsp_kstack[i]);

        while (get_pcpu_boot_info(i) == FALSE);

        KERN_INFO("[BSP KERN] done.\n");
    }

    all_ready = TRUE;

#ifdef TEST
    run_tests();
#else
    if (pcpu_ncpu() < 3) {
        /* Not enough CPUs: run producers and consumers on this CPU. */
        unsigned int pid;

        KERN_INFO("[BSP KERN] Fewer than 3 CPUs: running ping and pong on CPU 0.\n");
        pid = proc_create(_binary___obj_user_pingpong_ping_start, 1000);
        KERN_INFO("CPU0: process ping %d is created.\n", pid);
        KERN_INFO("CPU0: process pong %d is created.\n",
                  proc_create(_binary___obj_user_pingpong_pong_start, 1000));
        proc_create(_binary___obj_user_idle_idle_start, 1000);
        start_first_process(pid);
    }
#endif
}

static void kern_main_ap(void)
{
    int cpu_idx = get_pcpu_idx();

    set_pcpu_boot_info(cpu_idx, TRUE);

    while (all_ready == FALSE);

    KERN_INFO("[AP%d KERN] kernel_main_ap\n", cpu_idx);

    cpu_booted++;

#ifndef TEST
    unsigned int pid, pid2;

    if (pcpu_ncpu() < 3)
        return;

    if (cpu_idx == 1) {
        pid = proc_create(_binary___obj_user_pingpong_ping_start, 1000);
        KERN_INFO("CPU%d: process ping1 %d is created.\n", cpu_idx, pid);
        pid2 = proc_create(_binary___obj_user_pingpong_ping_start, 1000);
        KERN_INFO("CPU%d: process ping2 %d is created.\n", cpu_idx, pid2);
        proc_create(_binary___obj_user_idle_idle_start, 1000);
    }
    else if (cpu_idx == 2) {
        pid = proc_create(_binary___obj_user_pingpong_pong_start, 1000);
        KERN_INFO("CPU%d: process pong1 %d is created.\n", cpu_idx, pid);
        pid2 = proc_create(_binary___obj_user_pingpong_pong_start, 1000);
        KERN_INFO("CPU%d: process pong2 %d is created.\n", cpu_idx, pid2);
        proc_create(_binary___obj_user_idle_idle_start, 1000);
    }
    else
        return;

    start_first_process(pid);
#endif
}

void kern_init(uintptr_t mbi_addr)
{
    thread_init(mbi_addr);
    bbuf_init();
    KERN_INFO("[BSP KERN] Kernel initialized.\n");
    kern_main();
}

void kern_init_ap(void (*f)(void))
{
    devinit_ap();
    f();
}

#else  /* LAB == 7, or 8 for the course project */

extern uint32_t pcpu_ncpu(void);

static volatile int cpu_booted = 0;
static volatile int all_ready = FALSE;

extern uint8_t _binary___obj_user_idle_idle_start[];

static void kern_main(void)
{
    KERN_INFO("[BSP KERN] In kernel main.\n\n");

    KERN_INFO("[BSP KERN] Number of CPUs in this system: %d. \n", pcpu_ncpu());

    int cpu_idx = get_pcpu_idx();
    unsigned int pid;

    /* Lab 7 is evaluated with the file system test program user/fstest. */
    pid = proc_create(_binary___obj_user_idle_idle_start, 10000);
    KERN_INFO("CPU%d: process idle %d is created.\n", cpu_idx, pid);
    tqueue_remove(NUM_IDS, pid);
    tcb_set_state(pid, TSTATE_RUN);
    set_curid(pid);
    kctx_switch(0, pid);

    KERN_PANIC("kern_main() should never reach here.\n");
}

static void kern_main_ap(void)
{
    int cpu_idx = get_pcpu_idx();

    set_pcpu_boot_info(cpu_idx, TRUE);

    while (all_ready == FALSE);

    KERN_INFO("[AP%d KERN] kernel_main_ap\n", cpu_idx);

    cpu_booted++;
}

void kern_init(uintptr_t mbi_addr)
{
    thread_init(mbi_addr);
    bbuf_init();
    KERN_INFO("[BSP KERN] Kernel initialized.\n");
    kern_main();
}

void kern_init_ap(void (*f)(void))
{
    devinit_ap();
    f();
}

#endif
