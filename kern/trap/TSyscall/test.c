#include <lib/debug.h>
#include <lib/string.h>
#include <lib/syscall.h>
#include <lib/thread.h>
#include <lib/trap.h>
#include <lib/x86.h>
#include <lib/seg.h>
#include <pcpu/PCPUIntro/export.h>
#include <thread/PTCBIntro/export.h>
#include <thread/PThread/export.h>
#include "export.h"
#include "bbuf.h"

/* ---------------- Lab 5: sys_spawn ---------------- */

static void spawn(tf_t *tf, unsigned int elf_id, unsigned int quota)
{
    memzero(tf, sizeof(*tf));
    tf->regs.eax = SYS_spawn;
    tf->regs.ebx = elf_id;
    tf->regs.ecx = quota;
    sys_spawn(tf);
}

int TSyscall_test1()
{
    tf_t tf;

    spawn(&tf, 99, 100);
    if (tf.regs.eax != E_INVAL_PID || tf.regs.ebx != NUM_IDS) {
        dprintf("test 1.1 failed: invalid elf_id: (errno %d != %d || pid %d != %d)\n",
                tf.regs.eax, E_INVAL_PID, tf.regs.ebx, NUM_IDS);
        return 1;
    }
    spawn(&tf, 1, 100000000);
    if (tf.regs.eax != E_EXCEEDS_QUOTA || tf.regs.ebx != NUM_IDS) {
        dprintf("test 1.2 failed: quota too large: (errno %d != %d || pid %d != %d)\n",
                tf.regs.eax, E_EXCEEDS_QUOTA, tf.regs.ebx, NUM_IDS);
        return 1;
    }
    dprintf("test 1 passed.\n");
    return 0;
}

int TSyscall_test2()
{
    tf_t tf;
    unsigned int pid;

    spawn(&tf, 1, 100);
    pid = tf.regs.ebx;
    if (tf.regs.eax != E_SUCC || pid == 0 || pid >= NUM_IDS) {
        dprintf("test 2.1 failed: spawn ping: (errno %d != %d || pid = %d)\n",
                tf.regs.eax, E_SUCC, pid);
        return 1;
    }
    if (tcb_get_state(pid) != TSTATE_READY) {
        dprintf("test 2.2 failed: new process is not ready: (%d != %d)\n",
                tcb_get_state(pid), TSTATE_READY);
        return 1;
    }
    dprintf("test 2 passed.\n");
    return 0;
}

int test_TSyscall()
{
    return TSyscall_test1() + TSyscall_test2();
}

/* ---------------- Lab 6: bounded buffer ---------------- */

/* Spawns a kernel thread running entry (it must never return). */
static unsigned int spawn_kthread(void (*entry)(void))
{
    unsigned int pid = thread_spawn(entry, 0, 100);
    seg_init_proc(get_pcpu_idx(), pid);  // set up its kernel stack header
    return pid;
}

static volatile unsigned int got_item, got_flag, put_flag;

static void consumer_thread(void)
{
    got_item = bbuf_get();
    got_flag = 1;
    while (1)
        thread_yield();
}

static void producer_thread(void)
{
    bbuf_put(999);
    put_flag = 1;
    while (1)
        thread_yield();
}

int BBuffer_test1()
{
    unsigned int i, item;

    if (bbuf_count() != 0) {
        dprintf("test 1.1 failed: buffer not empty at start (%d)\n", bbuf_count());
        return 1;
    }
    for (i = 1; i <= 3; i++)
        bbuf_put(i);
    if (bbuf_count() != 3) {
        dprintf("test 1.2 failed: (count %d != 3)\n", bbuf_count());
        return 1;
    }
    for (i = 1; i <= 3; i++) {
        if ((item = bbuf_get()) != i) {
            dprintf("test 1.3 failed: not FIFO (got %d, expected %d)\n", item, i);
            return 1;
        }
    }
    if (bbuf_count() != 0) {
        dprintf("test 1.4 failed: (count %d != 0)\n", bbuf_count());
        return 1;
    }
    dprintf("test 1 passed.\n");
    return 0;
}

int BBuffer_test2()
{
    unsigned int pid = spawn_kthread(consumer_thread);

    thread_yield();  // the consumer runs and must block on the empty buffer
    if (got_flag != 0 || tcb_get_state(pid) != TSTATE_SLEEP) {
        dprintf("test 2.1 failed: consumer did not sleep on an empty buffer (state %d)\n",
                tcb_get_state(pid));
        return 1;
    }
    bbuf_put(42);
    if (tcb_get_state(pid) != TSTATE_READY) {
        dprintf("test 2.2 failed: bbuf_put did not wake up the consumer (state %d)\n",
                tcb_get_state(pid));
        return 1;
    }
    thread_yield();  // the consumer takes the item
    if (got_flag != 1 || got_item != 42 || bbuf_count() != 0) {
        dprintf("test 2.3 failed: (flag %d, item %d != 42, count %d)\n",
                got_flag, got_item, bbuf_count());
        return 1;
    }
    dprintf("test 2 passed.\n");
    return 0;
}

int BBuffer_test3()
{
    unsigned int i, item, pid;

    for (i = 0; i < BBUF_SIZE; i++)
        bbuf_put(100 + i);
    pid = spawn_kthread(producer_thread);
    thread_yield();  // the producer runs and must block on the full buffer
    if (put_flag != 0 || tcb_get_state(pid) != TSTATE_SLEEP
        || bbuf_count() != BBUF_SIZE) {
        dprintf("test 3.1 failed: producer did not sleep on a full buffer (state %d, count %d)\n",
                tcb_get_state(pid), bbuf_count());
        return 1;
    }
    if ((item = bbuf_get()) != 100) {
        dprintf("test 3.2 failed: (got %d != 100)\n", item);
        return 1;
    }
    thread_yield();  // the producer adds its item
    if (put_flag != 1 || bbuf_count() != BBUF_SIZE) {
        dprintf("test 3.3 failed: producer was not woken up (flag %d, count %d)\n",
                put_flag, bbuf_count());
        return 1;
    }
    for (i = 1; i < BBUF_SIZE; i++) {
        if ((item = bbuf_get()) != 100 + i) {
            dprintf("test 3.4 failed: (got %d != %d)\n", item, 100 + i);
            return 1;
        }
    }
    if ((item = bbuf_get()) != 999) {
        dprintf("test 3.5 failed: (got %d != 999)\n", item);
        return 1;
    }
    dprintf("test 3 passed.\n");
    return 0;
}

int test_BBuffer()
{
    return BBuffer_test1() || BBuffer_test2() || BBuffer_test3();
}
