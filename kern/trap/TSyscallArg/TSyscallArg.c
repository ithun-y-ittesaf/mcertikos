#include <lib/trap.h>
#include <lib/x86.h>

#include "import.h"

/**
 * Retrieves the system call arguments from the trap frame [tf] that get
 * passed in from the current running process' system call.
 * See the calling conventions in kern/lib/syscall.h.
 */
unsigned int syscall_get_arg1(tf_t *tf)
{
    // TODO [Lab 5]
    return 0;
}

unsigned int syscall_get_arg2(tf_t *tf)
{
    // TODO [Lab 5]
    return 0;
}

unsigned int syscall_get_arg3(tf_t *tf)
{
    // TODO [Lab 5]
    return 0;
}

unsigned int syscall_get_arg4(tf_t *tf)
{
    // TODO [Lab 5]
    return 0;
}

unsigned int syscall_get_arg5(tf_t *tf)
{
    // TODO [Lab 5]
    return 0;
}

unsigned int syscall_get_arg6(tf_t *tf)
{
    // TODO [Lab 5]
    return 0;
}

/**
 * Sets the error number in the trap frame [tf] that gets passed
 * to the current running process when we return to it.
 */
void syscall_set_errno(tf_t *tf, unsigned int errno)
{
    // TODO [Lab 5]
}

/**
 * Sets the return values in the trap frame [tf] that get passed
 * to the current running process when we return to it.
 */
void syscall_set_retval1(tf_t *tf, unsigned int retval)
{
    // TODO [Lab 5]
}

void syscall_set_retval2(tf_t *tf, unsigned int retval)
{
    // TODO [Lab 5]
}

void syscall_set_retval3(tf_t *tf, unsigned int retval)
{
    // TODO [Lab 5]
}

void syscall_set_retval4(tf_t *tf, unsigned int retval)
{
    // TODO [Lab 5]
}

void syscall_set_retval5(tf_t *tf, unsigned int retval)
{
    // TODO [Lab 5]
}
