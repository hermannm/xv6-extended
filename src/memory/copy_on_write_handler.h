#ifndef COPY_ON_WRITE_HANDLER_H
#define COPY_ON_WRITE_HANDLER_H

#include "../../lib/kernel/proc.h"
#include "../../lib/kernel/types.h"

/**
 * Implements copy-on-write handling of traps caused by writing to a page that has been made
 * readonly after a process fork.
 */
void handle_copy_on_write_to_page(uint64 faulting_virtual_address, struct proc *calling_process);

#endif // COPY_ON_WRITE_HANDLER_H
