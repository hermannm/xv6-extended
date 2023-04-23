#include "../../lib/kernel/riscv.h"
#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"

#include "main_thread.h"
#include "thread_list.h"

#include "thread.h"

static uint8 current_thread_id = MAIN_THREAD_ID;

struct thread *
create_thread(thread_function_t function, void *argument, uint32 result_size, uint32 stack_size)
{
    struct thread *thread = get_unused_thread();
    if (thread == 0) {
        return 0;
    }

    stack_size = stack_size > 0 ? stack_size : PGSIZE;
    void *stack = malloc(stack_size);
    if (stack == 0) {
        return 0;
    }
    // Adds the stack size to the pointer, since the stack "grows downward"
    uint64 stack_pointer = (uint64)stack + stack_size;

    struct thread_context context = {0};
    context.sp = stack_pointer;
    context.ra = (uint64)run_current_thread;

    thread->context = context;
    thread->state = THREAD_ACTIVE;
    thread->function = function;
    thread->argument = argument;
    thread->result = 0;
    thread->result_size = result_size;

    return thread;
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
    thread->state = THREAD_UNUSED;
    thread->function = 0;
    thread->argument = 0;
    thread->result = 0;
    thread->result_size = 0;
}

void yield_thread()
{
    struct thread *current_thread = get_thread(current_thread_id);
    struct thread *next_thread = get_next_active_thread(current_thread_id);

    int has_next_thread = next_thread != 0;

    if (has_next_thread && next_thread->id != current_thread_id) {
        current_thread_id = next_thread->id;
        tswtch(&current_thread->context, &next_thread->context);
    }

    if (!has_next_thread && current_thread->state == THREAD_EXITED) {
        struct thread *main_thread = get_thread(MAIN_THREAD_ID);

        int result;
        int error = get_thread_result(main_thread, &result, sizeof(int));

        free_thread_list();

        if (error == 0) {
            exit(result);
        } else {
            exit(1);
        }
    }
}

int join_thread(uint8 id_of_thread_to_join, void *result_buffer, uint32 result_size)
{
    struct thread *thread_to_join = get_thread(id_of_thread_to_join);
    if (thread_to_join == 0) {
        return -1;
    }

    while (thread_to_join->state != THREAD_EXITED) {
        yield_thread();
    }

    if (result_buffer != 0 && result_size != 0) {
        int error = get_thread_result(thread_to_join, result_buffer, result_size);
        if (error != 0) {
            return error;
        }
    }

    return 0;
}

int get_thread_result(struct thread *thread, void *result_buffer, uint32 result_size)
{
    void *result = thread->result;

    if (result == 0 || result_buffer == 0 || result_size != thread->result_size) {
        return -1;
    }

    memmove(result_buffer, thread->result, result_size);

    return 0;
}

uint8 get_current_thread_id()
{
    return current_thread_id;
}

void run_current_thread()
{
    struct thread *current_thread = get_thread(current_thread_id);
    if (current_thread == 0) {
        return;
    }

    void *result = current_thread->function(current_thread->argument);

    current_thread->result = result;
    current_thread->state = THREAD_EXITED;

    yield_thread();
}
