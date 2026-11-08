#include <lib/debug.h>
#include <lib/spinlock.h>
#include <lib/x86.h>
#include "import.h"

struct SContainer {
    int quota;      // maximum memory quota of the process
    int usage;      // the current memory usage of the process
    int parent;     // the id of the parent process
    int nchildren;  // the number of child processes
    int used;       // whether current container is used by a process
};

// mCertiKOS supports up to NUM_IDS processes
static struct SContainer CONTAINER[NUM_IDS];
static spinlock_t container_lks[NUM_IDS];

/**
 * Initializes the container data for the root process (the one with index 0).
 * The root process is the one that gets spawned first by the kernel.
 */
void container_init(unsigned int mbi_addr)
{
    unsigned int real_quota;
    unsigned int idx;
    // TODO [Lab 2]: define your local variables here.

    pmem_init(mbi_addr);
    real_quota = 0;

    /**
     * TODO [Lab 2]: Compute the available quota and store it into the variable real_quota.
     * It should be the number of the unallocated pages with the normal permission
     * in the physical memory allocation table.
     */

    KERN_DEBUG("\nreal quota: %d\n\n", real_quota);

    CONTAINER[0].quota = real_quota;
    CONTAINER[0].usage = 0;
    CONTAINER[0].parent = 0;
    CONTAINER[0].nchildren = 0;
    CONTAINER[0].used = 1;

    for (idx = 0; idx < NUM_IDS; idx++) {
        spinlock_init(&container_lks[idx]);
    }
}

// Get the id of parent process of process # [id].
unsigned int container_get_parent(unsigned int id)
{
    // TODO [Lab 2]
    return 0;
}

// Get the number of children of process # [id].
unsigned int container_get_nchildren(unsigned int id)
{
    // TODO [Lab 2]
    return 0;
}

// Get the maximum memory quota of process # [id].
unsigned int container_get_quota(unsigned int id)
{
    // TODO [Lab 2]
    return 0;
}

// Get the current memory usage of process # [id].
unsigned int container_get_usage(unsigned int id)
{
    // TODO [Lab 2]
    return 0;
}

// Determines whether the process # [id] can consume an extra
// [n] pages of memory. If so, returns 1, otherwise, returns 0.
unsigned int container_can_consume(unsigned int id, unsigned int n)
{
    // TODO [Lab 2]
    return 0;
}

/**
 * Dedicates [quota] pages of memory for a new child process.
 * You can assume it is safe to allocate [quota] pages
 * (the check is already done outside before calling this function).
 * Returns the container index for the new child process.
 */
unsigned int container_split(unsigned int id, unsigned int quota)
{
    unsigned int child, nc;

    spinlock_acquire(&container_lks[id]);

    nc = CONTAINER[id].nchildren;
    child = id * MAX_CHILDREN + 1 + nc;  // container index for the child process

    if (NUM_IDS <= child) {
        spinlock_release(&container_lks[id]);
        return NUM_IDS;
    }

    /**
     * TODO [Lab 2]: Update the container structure of both parent and child process appropriately.
     */

    spinlock_release(&container_lks[id]);

    return child;
}

/**
 * Allocates one more page for process # [id], given that this will not exceed the quota.
 * The container structure should be updated accordingly after the allocation.
 * Returns the page index of the allocated page, or 0 in the case of failure.
 *
 * Note: the container of process # [id] is protected by container_lks[id]
 * (lab 6); keep your code between the acquire and the release.
 */
unsigned int container_alloc(unsigned int id)
{
    unsigned int page_index = 0;

    spinlock_acquire(&container_lks[id]);

    /*
     * TODO [Lab 2]: Implement the function here.
     */

    spinlock_release(&container_lks[id]);

    return page_index;
}

// Frees the physical page and reduces the usage by 1.
void container_free(unsigned int id, unsigned int page_index)
{
    spinlock_acquire(&container_lks[id]);

    // TODO [Lab 2]

    spinlock_release(&container_lks[id]);
}
