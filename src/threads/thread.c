#include "../../lib/kernel/riscv.h"
#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"

#include "thread_list.h"

#include "thread.h"

static uint8 current_thread_id = MAIN_THREAD_ID;

void schedule_next_thread()
{
    struct thread *current_thread = get_thread(current_thread_id);
    struct thread *next_thread = get_next_runnable_thread(current_thread_id);

    if (next_thread != 0) {
        next_thread->state = RUNNING;
        current_thread_id = next_thread->id;
        tswtch(&current_thread->context, &next_thread->context);
    } else if (current_thread->state == EXITED) {
        struct thread *main_thread = get_thread(MAIN_THREAD_ID);

        int result = 1;
        get_thread_result(main_thread, &result, sizeof(int));

        free_thread(main_thread);

        exit(result);
    }
}

void create_thread(
    struct thread **returned_thread, struct thread_attributes *attributes,
    thread_function_t thread_function, void *thread_function_arg
)
{
    struct thread *thread = get_unused_thread();
    if (thread == 0) {
        return;
    }

    uint32 result_size = 0;
    uint32 stack_size = PGSIZE;
    if (attributes != 0) {
        struct thread_attributes given_attributes = *attributes;
        if (given_attributes.result_size != 0) {
            result_size = given_attributes.result_size;
        }
        if (given_attributes.stack_size != 0) {
            stack_size = given_attributes.stack_size;
        }
    }

    void *stack = malloc(stack_size);
    if (stack == 0) {
        return;
    }
    uint64 stack_pointer = (uint64)stack + stack_size;

    struct thread_context context = {0};
    context.sp = stack_pointer;
    context.ra = (uint64)run_current_thread;

    thread->context = context;
    thread->state = RUNNABLE;
    thread->function = thread_function;
    thread->argument = thread_function_arg;
    thread->result = 0;
    thread->result_size = result_size;

    *returned_thread = thread;
}

void free_thread(struct thread *thread)
{
    if (thread->result != 0) {
        free(thread->result);
    }
    if (thread->context.sp != 0) {
        free((void *)thread->context.sp);
    }

    thread->context = (struct thread_context){0};
    thread->state = UNUSED;
    thread->function = 0;
    thread->argument = 0;
    thread->result = 0;
    thread->result_size = 0;
}

void run_current_thread()
{
    struct thread *current_thread = get_thread(current_thread_id);
    if (current_thread == 0) {
        return;
    }

    void *result = current_thread->function(current_thread->argument);

    current_thread->result = result;
    current_thread->state = EXITED;

    schedule_next_thread();
}

void get_thread_result(struct thread *thread, void *result_buffer, uint32 result_size)
{
    if (result_size != thread->result_size) {
        return;
    }

    memmove(result_buffer, thread->result, result_size);

    return;
}

int join_thread(uint8 thread_id, void *result_buffer, uint32 result_size)
{
    struct thread *thread_to_join = get_thread(thread_id);
    if (thread_to_join == 0) {
        return -1;
    }

    while (thread_to_join->state != EXITED) {
        yield_thread();
    }

    if (result_buffer != 0 && result_size != 0) {
        if (thread_to_join->result_size != result_size) {
            return -1;
        }

        memmove(result_buffer, thread_to_join->result, result_size);
    }

    return 0;
}

void yield_thread()
{
    struct thread *current_thread = get_thread(current_thread_id);
    current_thread->state = RUNNABLE;
    schedule_next_thread();
}

uint8 get_current_thread_id()
{
    return current_thread_id;
}
