#ifndef THREAD_LIST_H
#define THREAD_LIST_H

#include "../../lib/kernel/types.h"

#include "thread.h"

void initialize_thread_list();

struct thread *get_thread(uint8 thread_id);

struct thread *get_unused_thread();

struct thread *get_next_runnable_thread(uint8 current_thread_id);

#endif // THREAD_LIST_H
