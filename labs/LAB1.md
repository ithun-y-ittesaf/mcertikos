# Lab 1: Physical Memory Allocator

## Objectives

- Understand how the kernel learns the physical memory layout from the BIOS memory map.
- Build the physical memory allocation table (AT).
- Implement a page allocator (`palloc`) and deallocator (`pfree`).

## Background

mCertiKOS manages physical memory in 4 KB pages. At boot, the bootloader
passes a memory map: a list of address ranges, each marked usable or
reserved. The kernel keeps one entry per physical page in the allocation
table `AT`. Each entry records the page's permission and whether the page is
allocated.

| `perm` value | Meaning |
|---|---|
| 0 | reserved by the BIOS |
| 1 | kernel only |
| > 1 | normal (available to processes) |

All addresses below `VM_USERLO` (1 GB) or at or above `VM_USERHI` (3.75 GB)
are reserved for the kernel.

The code is organised in layers. Each layer may use only the functions
listed in its `import.h`.

| Layer | File | Role |
|---|---|---|
| MATIntro | `kern/pmm/MATIntro/MATIntro.c` | getters and setters of the AT |
| MATInit | `kern/pmm/MATInit/MATInit.c` | builds the AT from the memory map |
| MATOp | `kern/pmm/MATOp/MATOp.c` | allocates and frees pages |

## Tasks

Find the tasks with `grep -rn "TODO \[Lab 1\]" kern`.

1. **MATIntro**: implement `at_is_norm`, `at_set_perm`, `at_is_allocated` and
   `at_set_allocated`.
2. **MATInit**: in `pmem_init`:
   - compute the number of physical pages `nps` from the memory map
     (`get_size`, `get_mms`, `get_mml`, `is_usable`);
   - initialise the permission of every page from 0 to `nps - 1`, following
     the rules in the comment.
3. **MATOp**:
   - implement `palloc`, first as a simple linear scan, then improved with
     memoization (remember where the last search ended);
   - implement `pfree`.

   Keep your code between `mem_lock()` and `mem_unlock()`.
4. *(optional)* Implement the `%o` (octal) format in `kern/lib/printfmt.c`
   and the `backtrace` monitor command in `kern/lib/monitor.c`.

## Running and testing

```
make LAB=1 TEST=1
```

Expected output when all tasks are correct:

```
Testing the MATIntro layer...
...
All tests passed.

Testing the MATInit layer...
...
All tests passed.

Testing the MATOp layer...
...
All tests passed.

==================================================
LAB 1 RESULT: 3 / 3 test groups passed.
==================================================
```

`make LAB=1 && make qemu-nox` boots to the kernel monitor. Try `help` and
`kerninfo`. Exit qemu with `Ctrl-a x`.

## Evaluation (10 marks)

| Item | Check | Marks |
|---|---|---|
| MATIntro | "Testing the MATIntro layer" passes | 2 |
| MATInit | "Testing the MATInit layer" passes | 3 |
| MATOp | "Testing the MATOp layer" passes, and `palloc` uses memoization | 3 |
| Viva | answers to the questions below | 2 |

## Viva questions

1. Why does `palloc` not need to scan pages below `VM_USERLO`?
2. A memory range covers only half of a page. What permission does the page get, and why?
3. What does memoization change in the running time of `palloc`?
