#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "../../lib/kernel/types.h"
#include "sys_getprocs.h"

struct process_list {
    struct process_info* array;
    uint length;
    uint capacity;
};

enum list_processes_error {
    LIST_PROCESSES_SUCCESS = 0,
    LIST_PROCESSES_SYSCALL_ERROR = -1,
};

struct list_processes_result {
    struct process_list processes;
    enum list_processes_error error;
};

struct list_processes_result list_processes();

void destroy_process_list(struct process_list);

void increase_process_list_capacity(struct process_list* proc_list, uint extra_capacity);

#endif // PROCESS_LIST_H
