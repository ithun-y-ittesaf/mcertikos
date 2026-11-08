#include <lib/debug.h>
#include <lib/trap.h>
#include <lib/syscall.h>
#include <dev/intr.h>
#include "export.h"

void exception_handler(tf_t *tf);
void interrupt_handler(tf_t *tf);
void syscall_dispatch(tf_t *tf);

static int check(int trapno, trap_cb_t expected, const char *name)
{
    if (TRAP_HANDLER[0][trapno] != expected) {
        dprintf("test failed: trap %d is not handled by %s\n", trapno, name);
        return 1;
    }
    return 0;
}

int TTrapInit_test1()
{
    if (check(T_DIVIDE, exception_handler, "exception_handler")
        || check(T_GPFLT, exception_handler, "exception_handler")
        || check(T_PGFLT, exception_handler, "exception_handler")
        || check(T_IRQ0 + IRQ_TIMER, interrupt_handler, "interrupt_handler")
        || check(T_IRQ0 + IRQ_KBD, interrupt_handler, "interrupt_handler")
        || check(T_IRQ0 + IRQ_IDE1, interrupt_handler, "interrupt_handler")
        || check(T_SYSCALL, syscall_dispatch, "syscall_dispatch"))
        return 1;
    dprintf("test 1 passed.\n");
    return 0;
}

int test_TTrapInit()
{
    return TTrapInit_test1();
}
