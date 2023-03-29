#ifndef PAGE_REFERENCE_COUNT_H
#define PAGE_REFERENCE_COUNT_H

#include "../../lib/kernel/types.h"

/**
 * Initializes the reference counts for pages.
 * Must be called before any of the other reference count functions.
 */
void initialize_page_reference_counts();

/**
 * Gets the reference count for the given allocation's memory page.
 */
uint get_page_reference_count(void *allocation);

/**
 * Adds 1 to the reference count for the given allocation's memory page, and returns the updated
 * reference count.
 */
uint increment_page_reference_count(void *allocation);

/**
 * Subtracts 1 from the reference count for the given allocation's memory page, and returns the
 * updated reference count.
 */
uint decrement_page_reference_count(void *allocation);

/**
 * Allocates and returns a page of memory, and sets its reference count to 1.
 */
void *allocate_page_with_reference_count();

/**
 * Decrements the reference count for the given allocation's memory page.
 * If it now has 0 references, frees the page.
 * Returns the updated reference count for the page.
 */
uint free_page_if_unreferenced(void *allocation);

/**
 * Converts the address of the given allocation to an index into the array of page reference counts.
 * Panics if the address is out-of-bounds of the reference count array.
 * Used internally by the other reference count functions.
 */
uint64 page_reference_count_index_from_pointer(void *allocation);

#endif // PAGE_REFERENCE_COUNT_H
