#include "lib/x86.h"

#include "import.h"

/**
 * Initializes all the thread queues with tqueue_init_at_id.
 * There are NUM_IDS + NUM_CPUS queues: queue # NUM_IDS + i is the ready
 * queue of CPU # i (with a single CPU, this is just queue # NUM_IDS).
 */
void tqueue_init(unsigned int mbi_addr)
{
    // TODO [Lab 4]: define your local variables here.

    tcb_init(mbi_addr);

    // TODO [Lab 4]
}

/**
 * Insert the TCB #pid into the tail of the thread queue #chid.
 * Recall that the doubly linked list is index based.
 * So you only need to insert the index.
 * Hint: there are multiple cases in this function.
 */
void tqueue_enqueue(unsigned int chid, unsigned int pid)
{
    // TODO [Lab 4]
}

/**
 * Reverse action of tqueue_enqueue, i.e. pops a TCB from the head of the specified queue.
 * It returns the popped thread's id, or NUM_IDS if the queue is empty.
 * Hint: there are multiple cases in this function.
 */
unsigned int tqueue_dequeue(unsigned int chid)
{
    // TODO [Lab 4]
    return 0;
}

/**
 * Removes the TCB #pid from the queue #chid.
 * Hint: there are many cases in this function.
 */
void tqueue_remove(unsigned int chid, unsigned int pid)
{
    // TODO [Lab 4]
}
