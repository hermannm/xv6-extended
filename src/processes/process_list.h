#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "../../lib/kernel/types.h"
#include "sys_getprocs.h"

struct process_list {
    struct process_info* array;
    uint length;
    uint capacity;
};

enum get_processes_error {
    GET_PROCESSES_SUCCESS = 0,
    GET_PROCESSES_SYSCALL_ERROR = -1,
};

struct get_processes_result {
    struct process_list processes;
    enum get_processes_error error;
};

struct get_processes_result get_processes();

void destroy_process_list(struct process_list);

void increase_process_list_capacity(struct process_list* proc_list, uint extra_capacity);

#endif // PROCESS_LIST_H
