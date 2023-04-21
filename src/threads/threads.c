#include "../../lib/kernel/types.h"

#include "threads.h"

void thread_scheduler()
{
    // TODO: Implement a userspace round robin scheduler that switches to the next thread
}

void create_thread(
    struct thread **thread, struct thread_attributes *attributes, thread_function_t thread_function,
    void *thread_function_args
)
{
    // TODO: Create a new process and add it as runnable, such that it starts running
    // once the scheduler schedules it the next time
}

int join_thread(int thread_id, void *status, uint size)
{
    // TODO: Wait for the thread with TID to finish. If status and size are non-zero,
    // copy the result of the thread to the memory, status points to. Copy size bytes.
    return 0;
}

void yield_thread()
{
    // TODO: Implement the yielding behaviour of the thread
}

uint8 get_current_thread_id()
{
    // TODO: Returns the thread id of the current thread
    return 0;
}
