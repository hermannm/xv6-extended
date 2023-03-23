#ifndef PAGE_REFERENCE_COUNT_H
#define PAGE_REFERENCE_COUNT_H

#include "../../lib/kernel/types.h"

void initialize_page_reference_counts();

uint get_page_reference_count(void *allocation);

void increment_page_reference_count(void *allocation);

void decrement_page_reference_count(void *allocation);

void free_page_or_decrement_reference_count(void *allocation);

uint64 page_reference_count_index_from_pointer(void *allocation);

#endif // PAGE_REFERENCE_COUNT_H
