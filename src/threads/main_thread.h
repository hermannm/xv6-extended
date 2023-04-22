#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#define MAIN_THREAD_ID 0

void _main(int argc, char *argv[]);

void *main_thread_function(void *arg);

struct main_thread_arg {
    int argc;
    char **argv;
};

#endif // MAIN_THREAD_H
