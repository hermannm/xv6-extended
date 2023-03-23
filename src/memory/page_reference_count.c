#include "page_reference_count.h"
#include "../../lib/kernel/defs.h"
#include "../../lib/kernel/memlayout.h"
#include "../../lib/kernel/proc.h"
#include "../../lib/kernel/riscv.h"
#include "../../lib/kernel/spinlock.h"
#include "../../lib/kernel/types.h"

#define MAX_PAGES (PHYSTOP - KERNBASE) / PGSIZE

static uint page_reference_counts[MAX_PAGES] = {0};
static struct spinlock page_reference_counts_lock;

void initialize_page_reference_counts()
{
    initlock(&page_reference_counts_lock, "page reference counts lock");
}

uint get_page_reference_count(void *allocation)
{
    acquire(&page_reference_counts_lock);

    uint64 index = page_reference_count_index_from_pointer(allocation);
    uint reference_count = page_reference_counts[index];

    release(&page_reference_counts_lock);

    return reference_count;
}

void increment_page_reference_count(void *allocation)
{
    acquire(&page_reference_counts_lock);

    uint64 index = page_reference_count_index_from_pointer(allocation);
    page_reference_counts[index]++;

    release(&page_reference_counts_lock);
}

void decrement_page_reference_count(void *allocation)
{
    acquire(&page_reference_counts_lock);

    uint64 index = page_reference_count_index_from_pointer(allocation);
    page_reference_counts[index]--;

    release(&page_reference_counts_lock);
}

void free_page_or_decrement_reference_count(void *allocation)
{
    uint page_reference_count = get_page_reference_count(allocation);

    if (page_reference_count <= 1) {
        kfree(allocation);
    } else {
        decrement_page_reference_count(allocation);
    }
}

uint64 page_reference_count_index_from_pointer(void *allocation)
{
    uint64 index = ((uint64)allocation - KERNBASE) / PGSIZE;

    if (index < 0 || index >= MAX_PAGES) {
        panic("page_reference_count_index_from_pointer received out-of-bounds physical address");
    }

    return index;
}
