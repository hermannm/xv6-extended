#include "sys_vatopa.h"
#include "../../lib/kernel/defs.h"
#include "../../lib/kernel/proc.h"
#include "../../lib/kernel/types.h"

extern struct proc proc[NPROC];

uint64 sys_vatopa(void)
{
    const struct vatopa_args args = get_vatopa_args();

    struct proc *process = 0;

    if (args.process_id == 0) {
        process = myproc();
        acquire(&process->lock);
    } else {
        for (process = proc; process < &proc[NPROC]; process++) {
            acquire(&process->lock);
            if (process->pid == args.process_id) {
                break;
            }
            release(&process->lock);
        }
    }

    if (process != 0) {
        uint64 physical_address = walkaddr(process->pagetable, args.virtual_address);
        release(&process->lock);
        return physical_address;
    } else {
        return 0;
    }
}

struct vatopa_args get_vatopa_args()
{
    struct vatopa_args args;

    argaddr(0, &args.virtual_address);
    argint(1, &args.process_id);

    return args;
}
