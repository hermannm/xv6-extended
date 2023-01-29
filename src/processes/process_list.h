#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "../../lib/kernel/types.h"
#include "sys_getprocs.h"

/**
 * A dynamically sized list of process info structs.
 */
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

/**
 * Makes syscalls to find all currently running processes, and returns a result struct with a
 * `process_list` and an error code.
 *
 * If the returned error code is not a success, the list is invalid and must NOT be used.
 *
 * If the function did not error, `destroy_process_list` must be called on the returned list to
 * properly dispose of it after use.
 */
struct get_processes_result get_processes();

/**
 * Frees the memory allocated for the given process list. List must not be used after this call.
 */
void destroy_process_list(struct process_list* proc_list);

/**
 * Allocates memory for `extra_capacity` more `process_info` structs in the given process list.
 */
void increase_process_list_capacity(struct process_list* proc_list, uint extra_capacity);

#endif // PROCESS_LIST_H
