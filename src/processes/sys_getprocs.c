#include "sys_getprocs.h"
#include "../../lib/kernel/defs.h"
#include "../../lib/kernel/param.h"
#include "../../lib/kernel/proc.h"
#include "../../lib/kernel/riscv.h"
#include "../../lib/kernel/spinlock.h"
#include "../../lib/kernel/types.h"

extern struct proc proc[NPROC];

extern struct spinlock wait_lock;

uint64 sys_getprocs(void)
{
    const struct getprocs_args args = get_getprocs_args();

    const struct proc *const calling_proc = myproc();
    const pagetable_t calling_proc_pagetable = calling_proc->pagetable;

    // wait_lock must be acquired to access proc.parent
    acquire(&wait_lock);

    int procs_added;
    for (procs_added = 0; procs_added < args.procs_to_get; procs_added++) {
        const int proc_index = args.offset + procs_added;
        if (proc_index >= NPROC) {
            break;
        }

        // `proc + proc_index` is done instead of `proc[proc_index]` in order to not dereference the
        // pointer, as that would copy the process's lock, which invalidates it.
        struct proc *const current_proc = proc + proc_index;

        acquire(&current_proc->lock);

        if (current_proc->state == UNUSED) {
            release(&current_proc->lock);
            break;
        }

        struct process_info proc_info = {
            .id = current_proc->pid,
            .status = current_proc->state,
        };
        strncpy(proc_info.name, current_proc->name, MAX_PROCESS_NAME_LENGTH);

        struct proc *const parent_proc = current_proc->parent;
        if (parent_proc) {
            acquire(&parent_proc->lock);
            proc_info.parent_id = parent_proc->pid;
            release(&parent_proc->lock);
        } else {
            proc_info.parent_id = -1;
        }

        release(&current_proc->lock);

        const uint64 target_address =
            args.proc_info_array_address + sizeof(struct process_info) * proc_index;

        const int err_code = copyout(
            calling_proc_pagetable, target_address, (char *)&proc_info, sizeof(struct process_info)
        );
        if (err_code == -1) {
            release(&wait_lock);
            return GETPROCS_MEMORY_COPY_ERROR;
        }
    }

    release(&wait_lock);

    return procs_added;
}

struct getprocs_args get_getprocs_args()
{
    struct getprocs_args args;

    argint(0, &args.procs_to_get);
    argint(1, &args.offset);
    argaddr(2, &args.proc_info_array_address);

    return args;
}
