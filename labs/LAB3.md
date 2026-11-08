# Lab 3: Virtual Memory Management

## Objectives

- Translate between virtual addresses and page table entries.
- Build the initial page structures of all processes, with an identity map for the kernel.
- Map and unmap pages on demand, and turn on paging.

## Background

This lab builds on MPTIntro (lab 2). Each layer adds one level of
abstraction.

| Layer | File | Role |
|---|---|---|
| MPTOp | `kern/vmm/MPTOp/MPTOp.c` | page directory/table operations by virtual address, and the identity page tables |
| MPTComm | `kern/vmm/MPTComm/MPTComm.c` | initial page directories of all processes; allocating and freeing page tables |
| MPTKern | `kern/vmm/MPTKern/MPTKern.c` | identity map for process 0 (the kernel); `map_page`, `unmap_page` |
| MPTNew | `kern/vmm/MPTNew/MPTNew.c` | allocates a page and maps it (used by the page fault handler) |

After `pdir_init_kern`, `paging_init` loads the page directory of process 0
and turns paging on.

## Prerequisite

Labs 1 and 2 are complete.

## Tasks

Find the tasks with `grep -rn "TODO \[Lab 3\]" kern`.

1. **MPTOp**:
   - implement the six `*_by_va` functions;
   - implement `idptbl_init`. Kernel pages get `PTE_P | PTE_W | PTE_G`; the
     other pages get `PTE_P | PTE_W`.
2. **MPTComm**: implement `pdir_init`, `alloc_ptbl` and `free_ptbl`.
3. **MPTKern**: implement `pdir_init_kern`, `map_page` and `unmap_page`.
4. **MPTNew**: implement `alloc_page`.

## Running and testing

```
make LAB=3 TEST=1
```

Expected output:

```
Testing the MPTOp layer...       All tests passed.
Testing the MPTComm layer...     All tests passed.
Testing the MPTKern layer...     All tests passed.
Testing the MPTNew layer...      All tests passed.
==================================================
LAB 3 RESULT: 4 / 4 test groups passed.
==================================================
```

Then check that the kernel still runs with paging enabled:

```
make LAB=3 && make qemu-nox
```

The kernel must reach the monitor prompt (`$>`) without a fault.

## Evaluation (10 marks)

| Item | Check | Marks |
|---|---|---|
| MPTOp | test passes | 2 |
| MPTComm | test passes | 2 |
| MPTKern | test passes | 2 |
| MPTNew | test passes | 1 |
| Paging on | `make LAB=3 && make qemu-nox` reaches the monitor | 1 |
| Viva | answers to the questions below | 2 |

## Viva questions

1. Why must the kernel part of every process's page directory be an identity map?
2. What happens if `map_page` is called for an address whose page table does not exist yet?
3. Why does `unmap_page` not free the page table?
