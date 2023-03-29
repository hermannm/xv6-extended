#ifndef SYS_VATOPA_H
#define SYS_VATOPA_H

#include "../../lib/kernel/types.h"

/**
 * Syscall to translate a given virtual address to its corresponding physical address.
 *
 * @param virtual_address The virtual address to translate.
 * @param process_id ID of the process whose page table should be used to translate the address, or
 * 0 if the calling process's page table should be used.
 * @return An integer that is either:
 * - the translated physical address
 * - 0, if the given virtual address was not mapped or the process ID was invalid
 */
uint64 vatopa(uint64 virtual_address, int process_id);

/**
 * Implementation of the `vatopa` syscall. The function signature here is required for syscalls,
 * but hides the actual arguments and return type. See `vatopa` for the actual signature.
 */
uint64 sys_vatopa(void);

/**
 * Arguments expected by the `vatopa` syscall, in order. See `vatopa` for documentation about
 * their use.
 */
struct vatopa_args {
    uint64 virtual_address;
    int process_id;
};

/**
 * Gets the expected arguments to the `vatopa` syscall.
 */
struct vatopa_args get_vatopa_args();

#endif // SYS_VATOPA_H
