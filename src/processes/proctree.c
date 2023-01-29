#include "../../lib/user/user.h"
#include "../utils/int_list.h"
#include "../utils/strings.h"
#include "process_list.h"
#include "sys_getprocs.h"

#define MAX_PROCTREE_DEPTH 30

void print_process_with_children(
    struct process_info proc, struct process_list proc_list, struct int_list* printed_ids, int depth
)
{
    if (int_list_contains(printed_ids, proc.id)) {
        return;
    }
    append_to_int_list(printed_ids, proc.id);

    if (depth > 0) {
        if (depth > MAX_PROCTREE_DEPTH) {
            printf("MAX PROCTREE DEPTH REACHED\n");
            return;
        }

        char prefix_line[MAX_PROCTREE_DEPTH * 2 + 1] = "";

        for (int i = 0; i < depth; i++) {
            if (i == depth - 1) {
                strncat(prefix_line, "|-", 2);
            } else {
                strncat(prefix_line, "  ", 2);
            }
        }

        printf("%s", prefix_line);
    }

    printf("%s (%d): %d\n", proc.name, proc.id, proc.status);

    for (int i = 0; i < proc_list.length; i++) {
        struct process_info other_proc = proc_list.array[i];

        if (other_proc.parent_id == proc.id) {
            print_process_with_children(other_proc, proc_list, printed_ids, depth + 1);
        }
    }
}

int main(int argc, char** argv)
{
    struct get_processes_result res = get_processes();
    if (res.error == GET_PROCESSES_SYSCALL_ERROR) {
        printf("[ERROR] Failed to list processes due to syscall error");
        exit(1);
    }

    struct int_list printed_ids = create_int_list(5);

    for (int i = 0; i < res.processes.length; i++) {
        struct process_info proc = res.processes.array[i];
        print_process_with_children(proc, res.processes, &printed_ids, 0);
    }

    destroy_process_list(res.processes);

    exit(0);
}
