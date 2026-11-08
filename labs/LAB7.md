# Lab 7: Disk and File System

## Objectives

- Handle disk interrupts so that processes can sleep while waiting for the disk.
- Implement directory lookup and path name resolution.
- Implement the file system calls `read`, `write`, `close` and `fstat`.

## Background

The file system (in `kern/fs/`) is a simplified version of the xv6 file
system. It has these layers:

| Layer | Role |
|---|---|
| block cache | caches disk blocks in memory |
| log | crash recovery |
| inodes | file metadata and contents |
| directories | files that contain directory entries |
| path names | e.g. `/a/b/c` |
| file descriptors | per-process handles to open files |

A process that reads a block calls `ide_rw`. This starts the disk and puts
the process to sleep (with your `thread_sleep` from lab 6) until the disk
interrupt arrives. The interrupt handler `ide_intr` wakes the process up.

The second qemu disk, `certikos_disk.img`, holds the file system. It is
copied fresh from `newfs/` every time you run `make qemu`, `make qemu-nox`
or `make TEST=1`.

| Task | File |
|---|---|
| Disk interrupt | `kern/trap/TTrapHandler/TTrapHandler.c` (`interrupt_handler`) |
| Directories | `kern/fs/dir.c` (`dir_lookup`, `dir_link`) |
| Path names | `kern/fs/path.c` (`skipelem`, `namex`) |
| File system calls | `kern/fs/sysfile.c` (`fdalloc`, `sys_read`, `sys_write`, `sys_close`, `sys_fstat`) |

Useful functions:

- inodes: `inode_read`, `inode_write`, `inode_get`, `inode_lock`, `inode_unlockput`;
- files: `file_read`, `file_write`, `file_stat`, `file_close`;
- open-file table: `tcb_get_openfiles`, `tcb_set_openfiles`;
- copying between the kernel and the user: `pt_copyin`, `pt_copyout`.

## Prerequisite

Labs 1–6 are complete, in particular `thread_sleep` and `thread_wakeup`.

## Tasks

Find the tasks with `grep -rn "TODO \[Lab 7\]" kern`.

1. **Disk interrupt**: in `interrupt_handler`, handle `T_IRQ0 + IRQ_IDE1` by
   calling `ide_intr()`, then `intr_eoi()`.
2. **Directories**: implement `dir_lookup` and `dir_link`.
3. **Path names**: implement `skipelem` (see the examples in the comment),
   then complete the loop in `namex`.
4. **File system calls**: implement `fdalloc`, `sys_read`, `sys_write`,
   `sys_close` and `sys_fstat`.
   - Check the file descriptor; on error, return `-1` with `E_BADF`.
   - Copy data through a kernel buffer with `pt_copyin` / `pt_copyout`.
   - Return the number of bytes in `retval1`.

## Running and testing

```
make LAB=7 TEST=1          (or: make LAB=7 && make qemu-nox)
```

The kernel starts `idle`, which spawns the file system test program
`user/fstest`. Each test prints a line ending in `ok`. The last test,
`bigdir`, takes a while.

```
*******usertests starting*******
=====test file usertests.ran does not exists: ok
=====small file test ok=====
=====big files ok=====
=====many creates, followed by unlink; ok=====
=====rmdot ok=====
=====fourteen ok=====
=====bigfile test ok=====
=====subdir ok=====
=====linktest ok=====
=====unlinkread ok=====
=====dir vs file OK=====
=====empty file name OK=====
=====bigdir ok=====
*******end of tests*******
```

If the output stops right after `fstest in process 4.`, the disk interrupt
or `thread_sleep` / `thread_wakeup` is not working.

## Evaluation (10 marks)

| Item | Check | Marks |
|---|---|---|
| Disk interrupt | fstest starts and prints the first lines | 1 |
| Small file, big files | `small file test ok`, `big files ok` | 2 |
| Creates / unlink, rmdot, fourteen | the three tests pass | 2 |
| bigfile, subdir, linktest, unlinkread | the four tests pass | 2 |
| dir vs file, empty file name, bigdir | the three tests pass, `end of tests` is printed | 1 |
| Viva | answers to the questions below | 2 |

## Viva questions

1. Trace what happens, from `sys_read` down to the disk, when a process reads a block that is not cached.
2. In `namex`, why is the parent directory unlocked before the next one is locked?
3. Why are user buffers copied with `pt_copyin` / `pt_copyout` and not accessed directly?
