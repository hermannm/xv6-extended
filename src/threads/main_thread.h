#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

/**
 * Since the main thread is created first, its thread ID is 0.
 */
#define MAIN_THREAD_ID 0

/**
 * Entrypoint of user-space programs.
 * Sets up the main thread with the program's main function, and starts its execution.
 */
void _main(int argc, char *argv[]);

/**
 * Wrapper around the main function of user-space programs, to match the signature of
 * thread_function_t (see thread.h).
 */
void *main_thread_function(void *wrapped_args);

/**
 * Struct to wrap the arguments to the main function of user-space programs, to pass them to the
 * main thread.
 */
struct main_thread_arg {
    int argc;
    char **argv;
};

#endif // MAIN_THREAD_H
