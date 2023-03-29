#ifndef TRAP_HANDLER_H
#define TRAP_HANDLER_H

#include "../../lib/kernel/proc.h"
#include "../../lib/kernel/types.h"

/**
 * Implements copy-on-write handling of traps caused by writing to a page that has been made
 * readonly after a process fork.
 */
void handle_write_to_readonly_page(uint64 faulting_virtual_address, struct proc *calling_process);

#endif // TRAP_HANDLER_H
