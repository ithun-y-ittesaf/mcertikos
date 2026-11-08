#include <lib/debug.h>
#include <lib/string.h>
#include <lib/trap.h>
#include "export.h"

int TSyscallArg_test1()
{
    tf_t tf;

    memzero(&tf, sizeof(tf));
    tf.regs.eax = 11;
    tf.regs.ebx = 22;
    tf.regs.ecx = 33;
    tf.regs.edx = 44;
    tf.regs.esi = 55;
    tf.regs.edi = 66;
    if (syscall_get_arg1(&tf) != 11 || syscall_get_arg2(&tf) != 22
        || syscall_get_arg3(&tf) != 33 || syscall_get_arg4(&tf) != 44
        || syscall_get_arg5(&tf) != 55 || syscall_get_arg6(&tf) != 66) {
        dprintf("test 1.1 failed: arguments are not read from eax, ebx, ecx, edx, esi, edi.\n");
        return 1;
    }
    dprintf("test 1 passed.\n");
    return 0;
}

int TSyscallArg_test2()
{
    tf_t tf;

    memzero(&tf, sizeof(tf));
    syscall_set_errno(&tf, 7);
    if (tf.regs.eax != 7) {
        dprintf("test 2.1 failed: errno is not stored in eax (%d != 7).\n", tf.regs.eax);
        return 1;
    }
    syscall_set_retval1(&tf, 1);
    syscall_set_retval2(&tf, 2);
    syscall_set_retval3(&tf, 3);
    syscall_set_retval4(&tf, 4);
    syscall_set_retval5(&tf, 5);
    if (tf.regs.ebx != 1 || tf.regs.ecx != 2 || tf.regs.edx != 3
        || tf.regs.esi != 4 || tf.regs.edi != 5) {
        dprintf("test 2.2 failed: return values are not stored in ebx, ecx, edx, esi, edi.\n");
        return 1;
    }
    if (tf.regs.eax != 7) {
        dprintf("test 2.3 failed: setting a return value changed errno.\n");
        return 1;
    }
    dprintf("test 2 passed.\n");
    return 0;
}

int test_TSyscallArg()
{
    return TSyscallArg_test1() + TSyscallArg_test2();
}
