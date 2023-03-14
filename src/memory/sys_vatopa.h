#ifndef SYS_VATOPA_H
#define SYS_VATOPA_H

#include "../../lib/kernel/types.h"

uint64 vatopa(uint64 virtual_address, int process_id);

uint64 sys_vatopa(void);

struct vatopa_args {
    uint64 virtual_address;
    int process_id;
};

struct vatopa_args get_vatopa_args();

#endif // SYS_VATOPA_H
