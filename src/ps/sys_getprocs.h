#ifndef SYS_GETPROCS_H
#define SYS_GETPROCS_H

#include "../../lib/kernel/types.h"

struct process_info {
    int id;
    int status;
    char name[16];
};

struct getprocs_args {
    int procs_to_get;
    int offset;
    uint64 proc_info_array_address;
};

enum getprocs_result {
    GETPROCS_MEMORY_COPY_ERROR = -1,
};

uint64 sys_getprocs(void);

struct getprocs_args get_getprocs_args();

#endif // SYS_GETPROCS_H
