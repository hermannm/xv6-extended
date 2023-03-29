#include "../../lib/kernel/defs.h"
#include "../../lib/kernel/proc.h"
#include "../../lib/kernel/riscv.h"
#include "../../lib/kernel/types.h"

#include "page_reference_count.h"

#include "trap_handler.h"

void handle_write_to_readonly_page(uint64 faulting_virtual_address, struct proc *calling_process)
{
    pagetable_t pagetable = calling_process->pagetable;

    pte_t *pagetable_entry = walk(pagetable, faulting_virtual_address, 0);
    if (pagetable_entry == 0) {
        printf("Failed to find page table entry in trapped process");
        setkilled(calling_process);
        return;
    }

    uint64 faulting_physical_address = PTE2PA(*pagetable_entry);

    int flags = PTE_FLAGS(*pagetable_entry);
    if ((flags & PTE_FORKED) == 0) {
        printf("SEGFAULT");
        setkilled(calling_process);
        return;
    }

    flags &= ~PTE_FORKED;
    flags |= PTE_W;

    void *allocation = allocate_page_with_reference_count();
    if (allocation == 0) {
        printf("Failed to allocate page for process trapped by write to forked page");
        setkilled(calling_process);
        return;
    }

    memmove(allocation, (void *)PGROUNDDOWN(faulting_physical_address), PGSIZE);
    uvmunmap(pagetable, PGROUNDDOWN(faulting_virtual_address), 1, 0);

    int mappages_result = mappages(
        pagetable, PGROUNDDOWN(faulting_virtual_address), PGSIZE, (uint64)allocation, flags
    );
    if (mappages_result != 0) {
        printf("Failed to map virtual addresses onto new physical addresses for process");
        free_page_if_unreferenced(allocation);
        setkilled(calling_process);
        return;
    }

    uint new_reference_count = free_page_if_unreferenced((void *)faulting_physical_address);
    if (new_reference_count == 1) {
        *pagetable_entry &= ~PTE_FORKED;
        *pagetable_entry |= PTE_W;
    }
}
