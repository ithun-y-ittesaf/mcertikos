# Lab 2: Memory Containers and Page Tables

## Objectives

- Enforce per-process memory quotas with containers.
- Understand the two-level x86 page table structure.
- Implement the basic page directory and page table operations.

## Background

**Containers.** Every process has a container that records:

- its memory `quota`;
- its current `usage`;
- its `parent`;
- its number of children.

A process can give part of its quota to a new child (`container_split`).
Process 0 starts with all available pages.

**Page tables.** On x86, a virtual address is split as follows:

| Bits | Meaning |
|---|---|
| 31–22 | page directory index (PDE) |
| 21–12 | page table index (PTE) |
| 11–0 | offset in the page |

mCertiKOS keeps one statically allocated page directory per process
(`PDirPool`). It also keeps a set of identity page tables (`IDPTbl`) that map
the kernel's memory one-to-one.

| Layer | File |
|---|---|
| MContainer | `kern/pmm/MContainer/MContainer.c` |
| MPTIntro | `kern/vmm/MPTIntro/MPTIntro.c` |

## Prerequisite

Lab 1 is complete.

## Tasks

Find the tasks with `grep -rn "TODO \[Lab 2\]" kern`.

1. **MContainer**:
   - in `container_init`, compute the quota of process 0 (the number of free
     normal pages);
   - implement the getters, `container_can_consume`, `container_split`,
     `container_alloc` and `container_free`.

   Keep your code between the `spinlock_acquire` and `spinlock_release` calls
   that are already there.
2. **MPTIntro**: implement the following, following the comments:
   - `set_pdir_base` (load CR3);
   - `get_pdir_entry`, `set_pdir_entry`, `set_pdir_entry_identity`, `rmv_pdir_entry`;
   - `get_ptbl_entry`, `set_ptbl_entry`, `set_ptbl_entry_identity`, `rmv_ptbl_entry`.

## Running and testing

```
make LAB=2 TEST=1
```

Expected output:

```
Testing the MContainer layer...
...
All tests passed.

Testing the MPTIntro layer...
...
All tests passed.

==================================================
LAB 2 RESULT: 2 / 2 test groups passed.
==================================================
```

## Evaluation (10 marks)

| Item | Check | Marks |
|---|---|---|
| MContainer | "Testing the MContainer layer" passes | 4 |
| MPTIntro | "Testing the MPTIntro layer" passes | 4 |
| Viva | answers to the questions below | 2 |

## Viva questions

1. What are the values of the permission bits `PTE_P`, `PTE_W` and `PTE_U`, and why are all three set in a page directory entry?
2. Given the entry value 409607 for physical page 100, explain where 409607 comes from.
3. Why is the child container index `id * MAX_CHILDREN + 1 + nchildren`?
