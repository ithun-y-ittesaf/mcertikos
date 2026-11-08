#include <lib/x86.h>
#include <lib/debug.h>
#include <lib/thread.h>
#include <thread/PTCBIntro/export.h>
#include <thread/PTQueueIntro/export.h>
#include <lib/spinlock.h>
#include <lib/seg.h>
#include <pcpu/PCPUIntro/export.h>
#include "export.h"

int PThread_test1()
{
    void *dummy_addr = (void *) 0;
    unsigned int chid = thread_spawn(dummy_addr, 0, 1000);
    if (tcb_get_state(chid) != TSTATE_READY) {
        dprintf("test 1.1 failed: (%d != %d)\n",
                tcb_get_state(chid), TSTATE_READY);
        return 1;
    }
    if (tqueue_get_tail(NUM_IDS) != chid) {
        dprintf("test 1.2 failed: (%d != %d)\n",
                tqueue_get_tail(NUM_IDS), chid);
        return 1;
    }
    dprintf("test 1 passed.\n");
    return 0;
}

/**
 * Write Your Own Test Script (optional)
 *
 * Come up with your own interesting test cases to challenge your classmates!
 * In addition to the provided simple tests, selected (correct and interesting) test functions
 * will be used in the actual grading of the lab!
 * Your test function itself will not be graded. So don't be afraid of submitting a wrong script.
 *
 * The test function should return 0 for passing the test and a non-zero code for failing the test.
 * Be extra careful to make sure that if you overwrite some of the kernel data, they are set back to
 * the original value. O.w., it may make the future test scripts to fail even if you implement all
 * the functions correctly.
 */
int PThread_test_own()
{
    // TODO [Lab 4] (optional)
    // dprintf("own test passed.\n");
    return 0;
}

int test_PThread()
{
    return PThread_test1() + PThread_test_own();
}

/* ---------------- Lab 6: sleep / wakeup ---------------- */

/* Spawns a kernel thread running entry (it must never return). */
static unsigned int spawn_kthread(void (*entry)(void))
{
    unsigned int pid = thread_spawn(entry, 0, 100);
    seg_init_proc(get_pcpu_idx(), pid);  // set up its kernel stack header
    return pid;
}

static spinlock_t sleep_lk;
static int sleep_chan, other_chan;
static volatile int sleep_cond, sleep_woke;

static void sleeper_thread(void)
{
    spinlock_acquire(&sleep_lk);
    if (!sleep_cond)
        thread_sleep(&sleep_chan, &sleep_lk);
    sleep_woke = 1;
    spinlock_release(&sleep_lk);
    while (1)
        thread_yield();
}

int PThreadSleep_test1()
{
    unsigned int pid;

    spinlock_init(&sleep_lk);
    pid = spawn_kthread(sleeper_thread);
    thread_yield();  // the sleeper runs and goes to sleep
    if (tcb_get_state(pid) != TSTATE_SLEEP || tcb_get_chan(pid) != &sleep_chan) {
        dprintf("test 1.1 failed: thread is not sleeping on its channel (state %d)\n",
                tcb_get_state(pid));
        return 1;
    }
    if (sleep_lk.lock != 0) {
        dprintf("test 1.2 failed: thread_sleep did not release the lock\n");
        return 1;
    }
    thread_wakeup(&other_chan);
    if (tcb_get_state(pid) != TSTATE_SLEEP) {
        dprintf("test 1.3 failed: woken up by a wakeup on another channel\n");
        return 1;
    }
    spinlock_acquire(&sleep_lk);
    sleep_cond = 1;
    thread_wakeup(&sleep_chan);
    spinlock_release(&sleep_lk);
    if (tcb_get_state(pid) != TSTATE_READY) {
        dprintf("test 1.4 failed: thread_wakeup did not make the thread ready (state %d)\n",
                tcb_get_state(pid));
        return 1;
    }
    thread_yield();  // the sleeper wakes up, holding the lock again
    if (sleep_woke != 1 || sleep_lk.lock != 0) {
        dprintf("test 1.5 failed: thread did not continue after wakeup\n");
        return 1;
    }
    dprintf("test 1 passed.\n");
    return 0;
}

int test_PThreadSleep()
{
    return PThreadSleep_test1();
}
