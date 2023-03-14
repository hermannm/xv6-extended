#include "sys_vatopa.h"
#include "../../lib/kernel/defs.h"
#include "../../lib/kernel/proc.h"
#include "../../lib/kernel/types.h"

extern struct proc proc[NPROC];

uint64 sys_vatopa(void)
{
    const struct vatopa_args args = get_vatopa_args();

    uint64 physical_address = 0;

    for (struct proc *process = proc; process < &proc[NPROC]; process++) {
        acquire(&process->lock);

        if (process->pid != args.process_id) {
            release(&process->lock);
            continue;
        }

        physical_address = walkaddr(process->pagetable, args.virtual_address);

        release(&process->lock);
        break;
    }

    return physical_address;
}

struct vatopa_args get_vatopa_args()
{
    struct vatopa_args args;

    argaddr(0, &args.virtual_address);
    argint(1, &args.process_id);

    return args;
}
