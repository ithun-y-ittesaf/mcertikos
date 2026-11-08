#include <lib/gcc.h>
#include <lib/x86.h>
#include <lib/kstack.h>

#include "import.h"

/**
 * Checks whether enough resources are available for allocation and
 * allocates memory for the new child thread, then sets the eip, and esp
 * of the thread states. The eip should be set to [entry], and the
 * esp should be set to the corresponding kernel stack TOP
 * (see proc_kstack in lib/kstack.h).
 * Don't forget the stack is going down from high address to low.
 * We do not care about the rest of states when a new thread starts.
 * The function returns the child thread (process) id.
 * In case of an error, return NUM_IDS.
 */
unsigned int kctx_new(void *entry, unsigned int id, unsigned int quota)
{
    // TODO [Lab 4]
    return 0;
}
