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
    struct getprocs_args args = get_getprocs_args();

    struct proc* calling_proc = myproc();
    pagetable_t calling_proc_pagetable = calling_proc->pagetable;

    int procs_added;
    for (procs_added = 0; procs_added < args.procs_to_get; procs_added++) {
        int proc_index = args.offset + procs_added;
        if (proc_index == NPROC) {
            break;
        }

        acquire(&wait_lock);

        struct proc current_proc = proc[proc_index];
        acquire(&current_proc.lock);

        if (current_proc.state == UNUSED) {
            release(&wait_lock);
            release(&current_proc.lock);
            break;
        }

        struct process_info proc_info = {
            .id = current_proc.pid,
            .status = current_proc.state,
        };
        strncpy(proc_info.name, current_proc.name, MAX_PROCESS_NAME_LENGTH);

        if (current_proc.parent) {
            acquire(&current_proc.parent->lock);
            proc_info.parent_id = current_proc.parent->pid;
            release(&current_proc.parent->lock);
        } else {
            proc_info.parent_id = -1;
        }

        release(&wait_lock);
        release(&current_proc.lock);

        uint64 target_address =
            args.proc_info_array_address + sizeof(struct process_info) * proc_index;

        int err_code = copyout(
            calling_proc_pagetable, target_address, (char*)&proc_info, sizeof(struct process_info)
        );
        if (err_code == -1) {
            return GETPROCS_MEMORY_COPY_ERROR;
        }
    }

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
