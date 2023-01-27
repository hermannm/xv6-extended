#include "../../lib/user/user.h"
#include "process_list.h"

int main(int argc, char** argv)
{
    struct list_processes_result res = list_processes();
    if (res.error == LIST_PROCESSES_SYSCALL_ERROR) {
        printf("[ERROR] Failed to list processes due to syscall error");
        exit(1);
    }

    for (int i = 0; i < res.processes.length; i++) {
        struct process_info proc = res.processes.array[i];
        printf("%s (%d): %d\n", proc.name, proc.id, proc.status);
    }

    destroy_process_list(res.processes);

    exit(0);
}
