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
 * Pointer to a function that a thread should run. Takes a void pointer argument and returns a
 * void pointer, the actual type of which will depend on the thread function.
 */
typedef void *(*thread_function_t)(void *);

/**
 * The thread struct will contain all additional information we require
 * to handle for the threads. You might need to store more information
 * to implement parts of the tasks. Feel free to extend the struct
 * accordingly.
 */
struct thread {
    uint8 id;
    struct thread_context context;
    enum thread_state state;
    int wait_count;

    thread_function_t function;
    void *argument;
    void *result;
    uint32 result_size;
};

/**
 * The thread scheduler, which will directly switch to the next thread.
 */
void schedule_next_thread(void);

/**
 * Allocates and initializes a new thread, and stores the newly allocated thread into the thread
 * pointer. The created thread will be immediately runnable, no further steps required.
 *
 * @param function Pointer to function to run in the thread (see thread_function_t docstring for
 * the expected function signature).
 * @param argument Argument to be passed to the thread function when invoked. Pass 0 if the function
 * does not use its argument. To pass multiple arguments, wrap them in a struct.
 * @param result_size Size of the result returned by the thread function. Pass 0 if the function
 * returns nothing.
 * @param stack_size The size of the stack to allocate for the thread. Pass 0 to default to the page
 * size.
 */
struct thread *
create_thread(thread_function_t function, void *argument, uint32 result_size, uint32 stack_size);

void free_thread(struct thread *thread);

void run_current_thread();

void get_thread_result(struct thread *thread, void *result_buffer, uint32 result_size);

/**
 * Joins the calling thread with the thread with thread id tid. (This means wait
 * for thread tid to finish execution before returning). After returning the result_buffer
 * will contain the return value of the function.
 * @param thread_id The thread id of the thread to join
 * @param result_buffer If not null and the thread has a return value, this should contain the
 * return value
 * @param result_size This denotes the size of the return value in bytes
 * @return
 */
int join_thread(uint8 thread_id, void *result_buffer, uint32 result_size);

/**
 * Yields to another thread.
 */
void yield_thread();

/**
 * Returns the ID of the currently running thread.
 */
uint8 get_current_thread_id();

#endif // THREAD_H
