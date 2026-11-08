#include <lib/spinlock.h>
#include <thread/PThread/export.h>

#include "bbuf.h"

static struct {
    spinlock_t lock;
    unsigned int items[BBUF_SIZE];
    unsigned int head;   // index of the oldest item
    unsigned int count;  // number of items in the buffer
} bbuf;

void bbuf_init(void)
{
    spinlock_init(&bbuf.lock);
    bbuf.head = 0;
    bbuf.count = 0;
}

/*
 * TODO [Lab 6]: implement bbuf_put and bbuf_get (see bbuf.h).
 * Protect the buffer with bbuf.lock. While the buffer is full (bbuf_put) or
 * empty (bbuf_get), wait with thread_sleep(<channel>, &bbuf.lock); after
 * changing the buffer, wake up the threads waiting for that change with
 * thread_wakeup(<channel>). Use the address of a field of bbuf as a channel,
 * a different one for "not full" and for "not empty".
 */
void bbuf_put(unsigned int item)
{
    // TODO [Lab 6]
}

unsigned int bbuf_get(void)
{
    unsigned int item = 0;

    // TODO [Lab 6]

    return item;
}

unsigned int bbuf_count(void)
{
    return bbuf.count;
}
