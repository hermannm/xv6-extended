#include "thread.h"
#include "thread_list.h"

#include "main_thread.h"

void _main(int argc, char *argv[])
{
    initialize_thread_list();

    struct thread *main_thread;
    struct thread_attributes attributes = {.result_size = sizeof(int)};
    struct main_thread_arg arg = {.argc = argc, .argv = argv};
    create_thread(&main_thread, &attributes, main_thread_function, &arg);
    if (main_thread == 0) {
        exit(1);
    }

    struct thread_context empty_context = {0};
    tswtch(&empty_context, &main_thread->context);
}

void *main_thread_function(void *untyped_arg)
{
    extern int main(int argc, char *argv[]);

    struct main_thread_arg *arg = (struct main_thread_arg *)untyped_arg;

    int *result_buffer = malloc(sizeof(int));
    if (result_buffer == 0) {
        return 0;
    }

    *result_buffer = main(arg->argc, arg->argv);

    return (void *)result_buffer;
}
