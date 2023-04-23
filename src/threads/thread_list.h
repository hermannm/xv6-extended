#ifndef THREAD_LIST_H
#define THREAD_LIST_H

#include "../../lib/kernel/types.h"

#include "thread.h"

/**
 * The maximum number of threads that a process can have at a time.
 * One will always be occupied by the main thread.
 */
#define MAX_NUMBER_OF_THREADS 64

/**
 * Initializes the global thread list with thread IDs.
 * Must be called before any other thread functions.
 */
void initialize_thread_list();

/**
 * Goes through every thread in the thread list, and frees them.
 */
void free_thread_list();

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
 * Finds an active thread next in line after the given thread ID.
 * Returns 0 if there are no active threads, including the current thread.
 */
struct thread *get_next_active_thread(uint8 current_thread_id);

#endif // THREAD_LIST_H
