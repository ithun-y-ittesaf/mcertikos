#ifndef _KERN_TRAP_TSYSCALL_BBUF_H_
#define _KERN_TRAP_TSYSCALL_BBUF_H_

#ifdef _KERN_

/*
 * A bounded buffer (lab 6) shared by all processes, used by the
 * sys_produce / sys_consume system calls.
 * It holds at most BBUF_SIZE items and hands them out in FIFO order.
 */
#define BBUF_SIZE 10

void bbuf_init(void);

/* Appends item to the buffer. Sleeps while the buffer is full. */
void bbuf_put(unsigned int item);

/* Removes and returns the oldest item. Sleeps while the buffer is empty. */
unsigned int bbuf_get(void);

/* Returns the number of items currently in the buffer. */
unsigned int bbuf_count(void);

#endif  /* _KERN_ */

#endif  /* !_KERN_TRAP_TSYSCALL_BBUF_H_ */
