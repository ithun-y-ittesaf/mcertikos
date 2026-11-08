# Lab 6: Multiprocessor and Synchronization

## Objectives

- Register trap handlers for every CPU.
- Implement `thread_sleep` and `thread_wakeup` (condition synchronization).
- Implement a bounded buffer that blocks when it is full or empty.
- Run producers and consumers on different CPUs.

## Background

From this lab on, the kernel starts all CPUs (qemu is started with 4).
Every CPU has its own trap handler table (`TRAP_HANDLER[cpu][trapno]`) and
its own ready queue. Shared data structures are protected by spinlocks. This
is why earlier labs placed your code between `spinlock_acquire` and
`spinlock_release`.

A thread that must wait for a condition calls
`thread_sleep(chan, lk)`. This function:

1. releases the spinlock `lk`;
2. puts the thread to sleep on the channel `chan` (any address);
3. switches to another thread;
4. re-acquires `lk` before returning.

`thread_wakeup(chan)` makes every thread sleeping on `chan` ready again.
`sched_lk` makes sleeping and waking atomic with respect to each other, so a
wakeup is never lost.

The bounded buffer (`kern/trap/TSyscall/bbuf.h`) holds at most `BBUF_SIZE`
items in FIFO order:

- `bbuf_put` sleeps while the buffer is full;
- `bbuf_get` sleeps while it is empty.

The user programs `ping` (producer) and `pong` (consumer) use it through the
`produce` and `consume` system calls.

| Task | File |
|---|---|
| Trap handler table | `kern/trap/TTrapInit/TTrapInit.c` |
| Sleep / wakeup | `kern/thread/PThread/PThread.c` |
| Bounded buffer | `kern/trap/TSyscall/bbuf.c` |
| Producer / consumer system calls | `kern/trap/TSyscall/TSyscall.c` |

## Prerequisite

Labs 1–5 are complete.

## Tasks

Find the tasks with `grep -rn "TODO \[Lab 6\]" kern`.

1. **TTrapInit**: in `trap_init`, register the handlers for this CPU with
   `trap_handler_register`:

   | Trap numbers | Handler |
   |---|---|
   | exceptions (`T_DIVIDE` … `T_SIMD`, and `T_SECEV`) | `exception_handler` |
   | hardware interrupts (`T_IRQ0 + IRQ_*`, and the local APIC vectors `T_LTIMER` … `T_PERFCTR`) | `interrupt_handler` |
   | `T_SYSCALL` | `syscall_dispatch` |

   (Until now, `trap()` routed traps by their numbers. From lab 6 on, only
   this table is used.)
2. **Sleep / wakeup**: implement `thread_sleep` and `thread_wakeup`.
   A woken thread goes back to the ready queue of its own CPU
   (`tcb_get_cpu`).
3. **Bounded buffer**: implement `bbuf_put` and `bbuf_get` in `bbuf.c`, using
   `bbuf.lock`, `thread_sleep` and `thread_wakeup`.
4. **System calls**: implement `sys_produce` and `sys_consume` with the
   bounded buffer:
   - the n-th item of process `pid` is `pid * 1000 + n`;
   - print `CPU <c>: Process <p>: Produced <item>`, or `... Consumed <item>`.

## Running and testing

**Part A: tests**

```
make LAB=6 TEST=1
```

Expected output:

```
Testing the TTrapInit layer...               All tests passed.
Testing the PThread (sleep/wakeup) layer...  All tests passed.
Testing the bounded buffer layer...          All tests passed.
==================================================
LAB 6 RESULT: 3 / 3 test groups passed.
==================================================
```

If the test run hangs, your `thread_sleep` (or the waiting loop in the
buffer) does not switch to another thread.

**Part B: producer / consumer**

```
make LAB=6 && make qemu-nox
```

Two `ping` processes (1 and 2) produce on CPU 1. Two `pong` processes (4 and
5) consume on CPU 2. Every item must be consumed exactly once, in the order
it entered the buffer:

```
CPU 1: Process 1: Produced 1000
CPU 1: Process 2: Produced 2000
CPU 2: Process 4: Consumed 1000
CPU 1: Process 1: Produced 1001
CPU 2: Process 4: Consumed 2000
...
```

The producers make more items than the consumers take, so once the buffer
is full the producers sleep forever. This is expected. On a machine with
fewer than 3 CPUs, the kernel runs one `ping` and one `pong` on CPU 0
instead.

## Evaluation (10 marks)

| Item | Check | Marks |
|---|---|---|
| TTrapInit | test passes | 1 |
| Sleep / wakeup | test passes | 2 |
| Bounded buffer | test passes (FIFO, blocks when empty, blocks when full) | 3 |
| Producer / consumer | Part B output: items consumed in FIFO order, no item lost or duplicated, no panic | 2 |
| Viva | answers to the questions below | 2 |

## Viva questions

1. Why must `thread_sleep` acquire `sched_lk` before releasing `lk`? Describe the lost-wakeup problem.
2. Why does `bbuf_get` use `while (count == 0)` and not `if (count == 0)`?
3. Why does every CPU need its own ready queue and its own trap handler table?
