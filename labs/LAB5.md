# Lab 5: Processes, Traps and System Calls

## Objectives

- Pass system call arguments and results through the trap frame.
- Implement the `spawn` and `yield` system calls.
- Route exceptions and interrupts to their handlers, including demand paging through the page fault handler.
- Run several user processes, each in its own address space.

## Background

**System calls.** A user process makes a system call with `int $48`.

- The system call number is passed in `eax`.
- The arguments are passed in `ebx`, `ecx`, `edx`, `esi` and `edi`.
- The error number is returned in `eax`.
- The return values are returned in `ebx`, `ecx`, `edx`, `esi` and `edi`.

See `kern/lib/syscall.h`. The kernel saves these registers in a trap frame
(`tf_t`).

**Traps.** `trap()` in `kern/trap/TTrapHandler/TTrapHandler.c` routes each
trap by its number:

| Trap numbers | Handler |
|---|---|
| exceptions | `exception_handler` |
| interrupts | `interrupt_handler` |
| `T_SYSCALL` | `syscall_dispatch` |

The timer interrupt calls `sched_update`, which preempts the running process
from time to time.

| Layer | File |
|---|---|
| TSyscallArg | `kern/trap/TSyscallArg/TSyscallArg.c` |
| TSyscall | `kern/trap/TSyscall/TSyscall.c` (`sys_spawn`, `sys_yield`) |
| TTrapHandler | `kern/trap/TTrapHandler/TTrapHandler.c` |

## Prerequisite

Labs 1–4 are complete.

## Tasks

Find the tasks with `grep -rn "TODO \[Lab 5\]" kern`.

1. **TSyscallArg**: implement `syscall_get_arg1` … `syscall_get_arg6`,
   `syscall_set_errno` and `syscall_set_retval1` … `syscall_set_retval5`.
2. **TSyscall**:
   - implement `sys_spawn`, including all the error cases listed in the comment;
   - implement `sys_yield`.
3. **TTrapHandler**:
   - implement `exception_handler`: page faults go to `pgflt_handler`,
     everything else goes to `default_exception_handler`;
   - implement `interrupt_handler`: spurious, timer and default interrupts.
4. *(optional)* Implement `sys_fork` with copy-on-write. Fill in the
   `PTE_COW` case of `pgflt_handler`, then launch `user/fork_test` from
   `user/idle/idle.c` with `spawn(5, 1000)`.

## Running and testing

**Part A: tests**

```
make LAB=5 TEST=1
```

Expected output:

```
Testing the TSyscallArg layer...           All tests passed.
Testing the TSyscall (sys_spawn) layer...  All tests passed.
==================================================
LAB 5 RESULT: 2 / 2 test groups passed.
==================================================
```

**Part B: user processes**

```
make LAB=5 && make qemu-nox
```

At the monitor prompt, type `startuser`. The `idle` process spawns `ping`,
`pong` and `ding`. All three write to the same virtual address
`0xe0000000`, but each one must read back its own value. The lines may
appear in a different order:

```
idle
ping in process 4.
pong in process 5.
ding in process 6.
ping started.
ping: the value at address e0000000: 0
ping: writing the value 100 to the address e0000000
...
ping: the new value at address e0000000: 100
pong: the new value at address e0000000: 200
ding: the new value at address e0000000: 300
```

## Evaluation (10 marks)

| Item | Check | Marks |
|---|---|---|
| TSyscallArg | test passes | 2 |
| sys_spawn | test passes | 2 |
| User processes | `startuser` output as above, and each process reads back its own value | 4 |
| Viva | answers to the questions below | 2 |
| *(bonus)* fork | `fork_test` prints parent/child/grandchild with copy-on-write | +2 |

## Viva questions

1. Why does the first access to `0xe0000000` cause a page fault, and who handles it?
2. Why do the three processes see different values at the same virtual address?
3. Which register holds the system call number, and which holds the error number?
