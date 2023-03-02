#ifndef SYS_GETPROCS_H
#define SYS_GETPROCS_H

#include "../../lib/kernel/types.h"

#define MAX_PROCESS_NAME_LENGTH 16

/**
 * Info data about a process from the kernel.
 */
struct process_info {
    int id;
    int parent_id; /** If process has no parent, `parent_id` is -1. */
    int status;
    char name[MAX_PROCESS_NAME_LENGTH];
};

/**
 * Possible error codes returned by the `getprocs` syscall.
 */
enum getprocs_error {
    /** Returned if `getprocs` failed to copy process info into userspace memory. */
    GETPROCS_MEMORY_COPY_ERROR = -1,
};

/**
 * Syscall to get information about running processes from the kernel.
 *
 * @param procs_to_get The number of processes to fetch information about.
 * @param offset Offset at which to begin getting process information. Can be used in repeated calls
 * to `getprocs` in order to get process information in batches.
 * @param proc_info_array Pointer to an array of `process_info` structs, which the caller of
 * `getprocs` must have already allocated with room for `procs_to_get` elements. The syscall copies
 * `process_info` structs into this address for use by the caller.
 * @return An integer that is either:
 * - an error code from the `getprocs_error` enum if the syscall errored, or if not,
 * - the number of `process_info` structs copied into `proc_info_array_address`
 *   (if this number is less than `procs_to_get`, the syscall has reached the last process)
 */
int getprocs(int procs_to_get, int offset, struct process_info *proc_info_array);

/**
 * Implementation of the `getprocs` syscall. The function signature here is required for syscalls,
 * but hides the actual arguments and return type. See `getprocs` for the actual signature.
 */
uint64 sys_getprocs(void);

/**
 * Arguments expected by the `getprocs` syscall, in order. See `getprocs` for documentation about
 * their use.
 */
struct getprocs_args {
    int procs_to_get;
    int offset;
    uint64 proc_info_array_address;
};

/**
 * Gets the expected arguments to the `getprocs` syscall.
 */
struct getprocs_args get_getprocs_args();

#endif // SYS_GETPROCS_H
