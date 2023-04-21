#ifndef THREADS_H
#define THREADS_H

#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"

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
    enum procstate state;
    thread_function_t thread_function;
    void *thread_function_args;
};

/**
 * Attributes that can be set when creating a thread.
 */
struct thread_attributes {
    uint32 stacksize;
    uint32 res_size;
};

/**
 * The thread scheduler, which will directly switch to the next thread.
 */
void thread_scheduler(void);

/**
 * Allocates and initializes a new thread, and stores the newly allocated thread into the thread
 * pointer. The created thread will be immediately runnable, no further steps required.
 *
 * @param thread Double pointer, such that the pointer can be modified to contain the pointer to the
 * newly allocated thread.
 * @param attributes Attributes that configure the thread as needed.
 * @param thread_function The pointer to the function that should be run in the thread.
 * @param thread_function_args If the function takes an argument this can be passed using the arg
 * argument.
 */
void create_thread(
    struct thread **thread, struct thread_attributes *attributes, thread_function_t thread_function,
    void *thread_function_args
);

/**
 * Joins the calling thread with the thread with thread id tid. (This means wait
 * for thread tid to finish execution before returning). After returning the status
 * will contain the return value of the function.
 * @param thread_id The thread id of the thread to join
 * @param status If not null and the thread has a return value, this should contain the return
 * value
 * @param size This denotes the size of the return value in bytes
 * @return
 */
int join_thread(int thread_id, void *status, uint size);

/**
 * Yields to another thread.
 */
void yield_thread();

/**
 * Returns the ID of the currently running thread.
 */
uint8 get_current_thread_id();

#endif // THREADS_H
