#ifndef THREAD_H
#define THREAD_H

#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"

#define MAIN_THREAD_ID 0

enum thread_state {
    UNUSED,
    USED,
    SLEEPING,
    RUNNABLE,
    RUNNING,
    ZOMBIE,
    EXITED,
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

    thread_function_t function;
    void *argument;
    void *result;
    uint32 result_size;
};

/**
 * Attributes that can be set when creating a thread.
 */
struct thread_attributes {
    uint32 result_size;
    uint32 stack_size;
};

/**
 * The thread scheduler, which will directly switch to the next thread.
 */
void schedule_next_thread(void);

/**
 * Allocates and initializes a new thread, and stores the newly allocated thread into the thread
 * pointer. The created thread will be immediately runnable, no further steps required.
 *
 * @param returned_thread Double pointer, such that the pointer can be modified to contain the
 * pointer to the newly allocated thread.
 * @param attributes Attributes that configure the thread as needed.
 * @param thread_function The pointer to the function that should be run in the thread.
 * @param thread_function_arg If the function takes an argument this can be passed using the arg
 * argument.
 */
void create_thread(
    struct thread **returned_thread, struct thread_attributes *attributes,
    thread_function_t thread_function, void *thread_function_arg
);

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
