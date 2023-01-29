#ifndef REALLOC_H
#define REALLOC_H

#include "../../lib/kernel/types.h"

/**
 * Allocates memory of size `new_size`, then moves memory equal to `old_size` from `old_pointer` to
 * the newly allocated pointer. Returns the pointer to the newly allocated memory.
 */
void* realloc(void* old_pointer, uint old_size, uint new_size);

#endif // REALLOC_H
