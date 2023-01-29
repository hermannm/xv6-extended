#ifndef INT_LIST_H
#define INT_LIST_H

#include "../../lib/kernel/types.h"

struct int_list {
    int* array;
    uint length;
    uint capacity;
};

struct int_list create_int_list(uint initial_capacity);

void destroy_int_list(struct int_list* list);

void append_to_int_list(struct int_list* list, int item);

int int_list_contains(struct int_list* list, int item);

#endif // INT_LIST_H
