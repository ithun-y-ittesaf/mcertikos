#include <lib/debug.h>
#include <lib/pmap.h>
#include <lib/types.h>
#include <lib/x86.h>
#include <lib/trap.h>
#include <lib/syscall.h>
#include <dev/intr.h>
#include <pcpu/PCPUIntro/export.h>

#include "import.h"
#include "bbuf.h"

static char sys_buf[NUM_IDS][PAGESIZE];

/**
 * Copies a string from user into buffer and prints it to the screen.
 * This is called by the user level "printf" library as a system call.
 */
void sys_puts(tf_t *tf)
{
    unsigned int cur_pid;
    unsigned int str_uva, str_len;
    unsigned int remain, cur_pos, nbytes;

    cur_pid = get_curid();
    str_uva = syscall_get_arg2(tf);
    str_len = syscall_get_arg3(tf);

    if (!(VM_USERLO <= str_uva && str_uva + str_len <= VM_USERHI)) {
        syscall_set_errno(tf, E_INVAL_ADDR);
        return;
    }

    remain = str_len;
    cur_pos = str_uva;

    while (remain) {
        if (remain < PAGESIZE - 1)
            nbytes = remain;
        else
            nbytes = PAGESIZE - 1;

        if (pt_copyin(cur_pid, cur_pos, sys_buf[cur_pid], nbytes) != nbytes) {
            syscall_set_errno(tf, E_MEM);
            return;
        }

        sys_buf[cur_pid][nbytes] = '\0';
#if LAB == 6
        KERN_INFO("From cpu %d: %s", get_pcpu_idx(), sys_buf[cur_pid]);
#else
        KERN_INFO("%s", sys_buf[cur_pid]);
#endif

        remain -= nbytes;
        cur_pos += nbytes;
    }

    syscall_set_errno(tf, E_SUCC);
}

extern uint8_t _binary___obj_user_pingpong_ping_start[];
extern uint8_t _binary___obj_user_pingpong_pong_start[];
extern uint8_t _binary___obj_user_pingpong_ding_start[];
extern uint8_t _binary___obj_user_fstest_fstest_start[];
extern uint8_t _binary___obj_user_fork_test_fork_test_start[];

/**
 * Spawns a new child process.
 * The user level library function sys_spawn (defined in user/include/syscall.h)
 * takes two arguments [elf_id] and [quota], and returns the new child process id
 * or NUM_IDS (as failure), with appropriate error number.
 * Currently, we have three user processes defined in user/pingpong/ directory,
 * ping, pong, and ding, plus user/fstest (lab 7) and user/fork_test.
 * The linker ELF addresses for those compiled binaries are defined above.
 * Since we do not yet have a file system implemented in mCertiKOS,
 * we statically load the ELF binaries into the memory based on the
 * first parameter [elf_id].
 * For example, ping, pong, ding, fstest and fork_test correspond to the
 * elf_ids 1, 2, 3, 4 and 5, respectively.
 * If the parameter [elf_id] is none of these, then it should return
 * NUM_IDS with the error number E_INVAL_PID. The same error case apply
 * when the proc_create fails.
 * Otherwise, you should mark it as successful, and return the new child process id.
 * Before creating the process, also check that
 *   - the current process can consume [quota] pages (else E_EXCEEDS_QUOTA),
 *   - the child id would not exceed NUM_IDS, i.e.
 *     curid * MAX_CHILDREN + 1 + MAX_CHILDREN <= NUM_IDS
 *     (else E_MAX_NUM_CHILDEN_REACHED),
 *   - the current process has fewer than MAX_CHILDREN children
 *     (else E_INVAL_CHILD_ID),
 * returning NUM_IDS in each of these error cases.
 */
void sys_spawn(tf_t *tf)
{
    // TODO [Lab 5]
}

/**
 * Yields to another thread/process.
 * The user level library function sys_yield (defined in user/include/syscall.h)
 * does not take any argument and does not have any return values.
 * Do not forget to set the error number as E_SUCC.
 */
void sys_yield(tf_t *tf)
{
    // TODO [Lab 5]
}

/**
 * Forks the current process (lab 5, optional).
 * The child gets a copy of the parent's address space. Instead of copying
 * every page, share the pages read-only and mark them with PTE_COW, then make
 * the private copy in the page fault handler (pgflt_handler in
 * kern/trap/TTrapHandler/TTrapHandler.c) when either process writes to it.
 * Returns the child's process id to the parent and 0 to the child.
 * The test program is user/fork_test (elf_id 5).
 */
void sys_fork(tf_t *tf)
{
    // TODO [Lab 5] (optional)
    syscall_set_errno(tf, E_INVAL_CALLNR);
}

/**
 * Lab 6: producer / consumer.
 * The user programs ping (producer) and pong (consumer) call these system
 * calls (see user/pingpong/). On a machine with 3 or more CPUs, ping runs
 * on CPU 1 and pong on CPU 2.
 *
 * sys_produce puts the next item of the current process into the bounded
 * buffer (bbuf.c); the n-th item (n = 0, 1, ...) of process # pid is
 * pid * 1000 + n. sys_consume takes one item out. Both print what they did:
 *     CPU <cpu>: Process <pid>: Produced <item>
 *     CPU <cpu>: Process <pid>: Consumed <item>
 * and set the error number to E_SUCC.
 */
static unsigned int nproduced[NUM_IDS];

void sys_produce(tf_t *tf)
{
    // TODO [Lab 6]
}

void sys_consume(tf_t *tf)
{
    // TODO [Lab 6]
}
