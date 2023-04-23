#ifndef THREAD_H
#define THREAD_H

#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"

enum thread_state {
    THREAD_UNUSED,
    THREAD_RUNNABLE,
    THREAD_RUNNING,
    THREAD_EXITED,
};

/**
 * Pointer to a function to run in a thread. Takes a void pointer argument and returns a void
 * pointer, the actual type of which will depend on the thread function.
 */
typedef void *(*thread_function_t)(void *);

/**
 * A thread runs a function concurrently with other threads in a process.
 */
struct thread {
    uint8 id;
    struct thread_context context;
    enum thread_state state;

    /**
     * The number of other threads waiting for this thread to exit. While more than 0, the thread
     * will not be freed, so that its result can be retrieved by waiting threads.
     */
    int wait_count;

    thread_function_t function;
    void *argument;
    void *result;
    uint32 result_size;
};

/**
 * Finds the next runnable thread after the current thread, and switches to it.
 * If there are no other runnable threads, continues on the current thread.
 * If the current thread has also exited, retrieves the result of the main thread and exits the
 * program.
 */
void schedule_next_thread(void);

/**
 * Initializes a new thread, and marks it as runnable.
 *
 * @param function Pointer to function to run in the thread (see thread_function_t docstring for
 * the expected function signature).
 * @param argument Argument to be passed to the thread function when invoked. Pass 0 if the function
 * does not use its argument. To pass multiple arguments, wrap them in a struct.
 * @param result_size Size of the result returned by the thread function. Pass 0 if the function
 * returns nothing.
 * @param stack_size The size of the stack to allocate for the thread. Pass 0 to default to the page
 * size.
 * @return Pointer to the new thread. Once the thread exits, and there are no other threads waiting
 * on it, it will be automatically freed.
 */
struct thread *
create_thread(thread_function_t function, void *argument, uint32 result_size, uint32 stack_size);

/**
 * Frees the allocations on the thread, sets all its fields to 0 (except the thread ID), and marks
 * it as unused. Called automatically once the thread exits and there are no other threads waiting
 * on it, so should not be called separately.
 */
void free_thread(struct thread *thread);

/**
 * Gets the current thread, and runs its function. Once it completes, saves its result in the
 * thread struct, marks it as exited, and calls schedule_next_thread().
 */
void run_current_thread();

/**
 * Sets the current thread from running to runnable, then calls the scheduler to allow other threads
 * to run.
 */
void yield_thread();

/**
 * Yields to the thread with the given ID until it exits.
 * If the given result buffer is not 0, copies the joined thread's result into the buffer.
 * Returns 0 on success, or -1 if joining failed, either due to invalid thread ID or invalid result
 * buffer.
 */
int join_thread(uint8 id_of_thread_to_join, void *result_buffer, uint32 result_size);

/**
 * Copies the result of the thread's function into the given result buffer.
 * Returns 0 on success, or -1 if there either was no result or the provided result size did not
 * match the thread's result.
 */
int get_thread_result(struct thread *thread, void *result_buffer, uint32 result_size);

/**
 * Returns the ID of the currently running thread.
 */
uint8 get_current_thread_id();

#endif // THREAD_H
