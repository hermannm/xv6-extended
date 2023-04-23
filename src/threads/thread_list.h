#ifndef THREAD_LIST_H
#define THREAD_LIST_H

#include "../../lib/kernel/types.h"

#include "thread.h"

/**
 * Initializes the global thread list with thread IDs.
 * Must be called before any other thread functions.
 */
void initialize_thread_list();

/**
 * Returns the thread with the given thread ID, or 0 if no thread with that ID is found.
 */
struct thread *get_thread(uint8 thread_id);

/**
 * Returns the first available unused thread.
 * If an exited thread is found with no-one waiting on it, frees it and returns that.
 * Returns 0 if there are no available threads.
 */
struct thread *get_unused_thread();

/**
 * Finds a runnable thread next in line after the given thread ID.
 * Returns 0 if there are no runnable threads, including the current thread.
 */
struct thread *get_next_runnable_thread(uint8 current_thread_id);

#endif // THREAD_LIST_H
