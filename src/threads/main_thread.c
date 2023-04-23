#include "thread.h"
#include "thread_list.h"

#include "main_thread.h"

void _main(int argc, char *argv[])
{
    initialize_thread_list();

    struct main_thread_arg arg = {.argc = argc, .argv = argv};
    struct thread *main_thread = create_thread(main_thread_function, &arg, sizeof(int), 0);
    if (main_thread == 0) {
        exit(1);
    }

    main_thread->wait_count++;

    struct thread_context empty_context = {0};
    tswtch(&empty_context, &main_thread->context);
}

void *main_thread_function(void *wrapped_args)
{
    extern int main(int argc, char *argv[]);

    struct main_thread_arg *arg = (struct main_thread_arg *)wrapped_args;

    int *result_buffer = malloc(sizeof(int));
    if (result_buffer == 0) {
        return 0;
    }

    *result_buffer = main(arg->argc, arg->argv);

    return (void *)result_buffer;
}
