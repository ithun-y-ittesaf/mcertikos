# Lab 4: Kernel Threads and Scheduling

## Objectives

- Create kernel thread contexts.
- Manage thread control blocks (TCBs) and index-based doubly linked thread queues.
- Implement round-robin scheduling with `thread_spawn` and `thread_yield`.

## Background

Every thread has:

- a kernel context: the saved registers `esp`, `edi`, `esi`, `ebx`, `ebp`, `eip`;
- a TCB, which holds its state (`READY`, `RUN`, `SLEEP`, `DEAD`) and the
  `prev`/`next` indices used to link it into a queue.

Queues store thread ids, not pointers. The value `NUM_IDS` means "none".
There are `NUM_IDS + NUM_CPUS` queues: queue `NUM_IDS + i` is the ready
queue of CPU `i`. With one CPU, the ready queue is simply `NUM_IDS`.

The context switch itself, `cswitch` in
`kern/thread/PKCtxIntro/cswitch.S`, is provided. Read it to see how the
registers are saved and restored. You do not have to write any assembly.

| Layer | File |
|---|---|
| PKCtxNew | `kern/thread/PKCtxNew/PKCtxNew.c` |
| PTCBInit | `kern/thread/PTCBInit/PTCBInit.c` |
| PTQueueInit | `kern/thread/PTQueueInit/PTQueueInit.c` |
| PThread | `kern/thread/PThread/PThread.c` (`thread_spawn`, `thread_yield`) |

## Prerequisite

Labs 1–3 are complete.

## Tasks

Find the tasks with `grep -rn "TODO \[Lab 4\]" kern`.

1. **PKCtxNew**: implement `kctx_new`:
   - check the quota and allocate the child's container;
   - set the child's `eip` to the entry point and its `esp` to the top of its
     kernel stack.
2. **PTCBInit**: implement `tcb_init`.
3. **PTQueueInit**: implement `tqueue_init`, `tqueue_enqueue`,
   `tqueue_dequeue` and `tqueue_remove`. Consider all the cases: the queue
   is empty, the thread is the head, the thread is the tail, and the thread
   is in the middle.
4. **PThread**: implement `thread_spawn` and `thread_yield`.
   - Use the ready queue `NUM_IDS + get_pcpu_idx()`.
   - Release `sched_lk` before `kctx_switch`, and also on the path where no
     switch happens.

## Running and testing

```
make LAB=4 TEST=1
```

Expected output:

```
Testing the PKCtxNew layer...     All tests passed.
Testing the PTCBInit layer...     All tests passed.
Testing the PTQueueInit layer...  All tests passed.
Testing the PThread layer...      All tests passed.
==================================================
LAB 4 RESULT: 4 / 4 test groups passed.
==================================================
```

## Evaluation (10 marks)

| Item | Check | Marks |
|---|---|---|
| PKCtxNew | test passes | 2 |
| PTCBInit | test passes | 1 |
| PTQueueInit | test passes | 3 |
| PThread | test passes | 2 |
| Viva | answers to the questions below | 2 |

## Viva questions

1. Explain, using `cswitch.S`, how execution continues in the new thread after `kctx_switch`.
2. Why does `kctx_new` set `esp` to the top (the highest address) of the stack?
3. What happens in `thread_yield` when the current thread is the only ready thread?
