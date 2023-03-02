#include "int_list.h"
#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"
#include "realloc.h"

struct int_list create_int_list(const uint initial_capacity)
{
    struct int_list list = {
        .array = malloc(initial_capacity * sizeof(struct int_list)),
        .length = 0,
        .capacity = initial_capacity,
    };

    return list;
}

void destroy_int_list(struct int_list* list)
{
    free(list->array);
}

void append_to_int_list(struct int_list* const list, const int item)
{
    if (list->length >= list->capacity) {
        const uint old_capacity = list->capacity;
        list->capacity *= 2;

        list->array = realloc(
            list->array, old_capacity * sizeof(struct int_list),
            list->capacity * sizeof(struct int_list)
        );
    }

    list->array[list->length] = item;
    list->length++;
}

int int_list_contains(const struct int_list* const list, const int item)
{
    for (int i = 0; i < list->length; i++) {
        if (list->array[i] == item) {
            return 1;
        }
    }

    return 0;
}
