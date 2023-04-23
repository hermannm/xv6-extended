#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"

#include "main_thread.h"
#include "thread.h"

#include "thread_list.h"

static struct thread thread_list[MAX_NUMBER_OF_THREADS] = {0};

void initialize_thread_list()
{
    for (uint8 thread_id = 0; thread_id < MAX_NUMBER_OF_THREADS; thread_id++) {
        struct thread *thread = &thread_list[thread_id];
        thread->id = thread_id;
    }
}

void free_thread_list()
{
    for (struct thread *thread = thread_list; thread < &thread_list[MAX_NUMBER_OF_THREADS];
         thread++)
    {
        free_thread(thread);
    }
}

struct thread *get_thread(uint8 thread_id)
{
    if (thread_id >= MAX_NUMBER_OF_THREADS) {
        return 0;
    }

    return &thread_list[thread_id];
}

struct thread *get_unused_thread()
{
    for (struct thread *thread = thread_list; thread < &thread_list[MAX_NUMBER_OF_THREADS];
         thread++)
    {
        if (thread->state == THREAD_UNUSED) {
            return thread;
        }
    }

    return 0;
}

struct thread *get_next_runnable_thread(uint8 current_thread_id)
{
    uint8 index = current_thread_id + 1;
    while (1) {
        if (index >= MAX_NUMBER_OF_THREADS) {
            index = 0;
        }

        struct thread *thread = &thread_list[index];
        if (thread->state == THREAD_RUNNABLE) {
            return thread;
        }

        if (index == current_thread_id) {
            return 0;
        }

        index++;
    }
}
