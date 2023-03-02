#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"

void *realloc(void *old_pointer, const uint old_size, const uint new_size)
{
    void *new_pointer = malloc(new_size);

    uint mem_size_to_move;
    if (old_size < new_size) {
        mem_size_to_move = old_size;
    } else {
        mem_size_to_move = new_size;
    }

    new_pointer = memmove(new_pointer, old_pointer, mem_size_to_move);
    free(old_pointer);

    return new_pointer;
}
