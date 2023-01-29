#ifndef INT_LIST_H
#define INT_LIST_H

#include "../../lib/kernel/types.h"

/**
 * A dynamically sized list of integers, using a backing array that is resized as needed.
 */
struct int_list {
    int* array;
    uint length;
    uint capacity;
};

/**
 * Creates an empty integer list, allocated with a backing array with the given initial capacity.
 *
 * `destroy_int_list` must be called to properly dispose of the integer list after use.
 */
struct int_list create_int_list(uint initial_capacity);

/**
 * Frees the memory allocated for the given integer list. List must not be used after this call.
 */
void destroy_int_list(struct int_list* list);

/**
 * Appends the given item to the end of the integer list, incrementing the list's length.
 * If the list has reached its capacity, the backing array is reallocated to twice its size.
 */
void append_to_int_list(struct int_list* list, int item);

/**
 * Goes through the list, and returns 1 if any integer in the list matches the given item, else
 * returns 0.
 */
int int_list_contains(struct int_list* list, int item);

#endif // INT_LIST_H
