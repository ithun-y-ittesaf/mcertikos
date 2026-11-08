#include <lib/x86.h>
#include <lib/thread.h>
#include <lib/spinlock.h>
#include <lib/debug.h>
#include <dev/lapic.h>
#include <pcpu/PCPUIntro/export.h>
#include <kern/thread/PTCBIntro/export.h>

#include "import.h"

static spinlock_t sched_lk;

unsigned int sched_ticks[NUM_CPUS];

void thread_init(unsigned int mbi_addr)
{
    unsigned int i;
    for (i = 0; i < NUM_CPUS; i++) {
        sched_ticks[i] = 0;
    }

    spinlock_init(&sched_lk);
    tqueue_init(mbi_addr);
    set_curid(0);
    tcb_set_state(0, TSTATE_RUN);
}

/**
 * Allocates a new child thread context, sets the state of the new child thread
 * to ready, and pushes it to the ready queue.
 * It returns the child thread id.
 *
 * The ready queue of the current CPU is NUM_IDS + get_pcpu_idx() (just
 * NUM_IDS while there is a single CPU). Also record the CPU of the new
 * thread with tcb_set_cpu.
 * The scheduler state is protected by sched_lk (lab 6); keep your code
 * between the acquire and the release.
 */
unsigned int thread_spawn(void *entry, unsigned int id, unsigned int quota)
{
    unsigned int pid = NUM_IDS;

    spinlock_acquire(&sched_lk);

    // TODO [Lab 4]

    spinlock_release(&sched_lk);

    return pid;
}

/**
 * Yield to the next thread in the ready queue.
 * You should set the currently running thread state as ready,
 * and push it back to the ready queue.
 * Then set the state of the popped thread as running, set the
 * current thread id, and switch to the new kernel context.
 * Hint: If you are the only thread that is ready to run,
 * do you need to switch to yourself?
 * Note: the scheduler state is protected by sched_lk (lab 6). Release it
 * with spinlock_release(&sched_lk) before calling kctx_switch, and also on
 * the path where no switch is needed.
 */
void thread_yield(void)
{
    spinlock_acquire(&sched_lk);

    // TODO [Lab 4]
}

void sched_update(void)
{
    spinlock_acquire(&sched_lk);
    sched_ticks[get_pcpu_idx()] += 1000 / LAPIC_TIMER_INTR_FREQ;
    if (sched_ticks[get_pcpu_idx()] >= SCHED_SLICE) {
        sched_ticks[get_pcpu_idx()] = 0;
        spinlock_release(&sched_lk);
        thread_yield();
    }
    else {
        spinlock_release(&sched_lk);
    }
}

/**
 * Atomically release lock and sleep on chan.
 * Reacquires lock when awakened.
 * Record the channel with tcb_set_chan and set the state to TSTATE_SLEEP,
 * then switch to the next thread in the ready queue of the current CPU
 * (NUM_IDS + get_pcpu_idx()), as in thread_yield.
 */
void thread_sleep(void *chan, spinlock_t *lk)
{
    // TODO [Lab 6]: your local variables here.

    if (lk == 0)
        KERN_PANIC("sleep without lock");

    // TODO [Lab 6]:
    // Must acquire sched_lk in order to change the current thread's state and
    // then switch. Once we hold sched_lk, we can be guaranteed that we won't
    // miss any wakeup (wakeup runs with sched_lk locked), so it's okay to
    // release lock.

    // TODO [Lab 6]: Go to sleep.

    // TODO [Lab 6]: Context switch. (Release sched_lk before kctx_switch.)

    // TODO [Lab 6]: Tidy up.

    // TODO [Lab 6]: Reacquire original lock.
}

/**
 * Wake up all processes sleeping on chan.
 * Each of them becomes ready and goes back to the ready queue of the CPU it
 * ran on (NUM_IDS + tcb_get_cpu(pid)). Hold sched_lk while doing this.
 */
void thread_wakeup(void *chan)
{
    // TODO [Lab 6]
}
