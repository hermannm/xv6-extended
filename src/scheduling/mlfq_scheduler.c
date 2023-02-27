#include "mlfq_scheduler.h"
#include "../../lib/kernel/defs.h"
#include "../../lib/kernel/proc.h"
#include "../../lib/kernel/riscv.h"

#define QUEUE_COUNT 3

#define HIGHEST_PRIORITY_QUEUE 0
#define LOWEST_PRIORITY_QUEUE QUEUE_COUNT - 1

#define UNUSED_PROCESS 0

#define SCHEDULER_ITERATION_PRIORITY_RESET_INTERVAL 5

extern struct proc proc[NPROC];

void mlfq_scheduler(void)
{
    // Array of arrays of process IDs, static to maintain state between scheduler iterations.
    static int queues[QUEUE_COUNT][QUEUE_LENGTH] = {UNUSED_PROCESS};

    // Number of times the scheduler has run since the last priority reset.
    static int scheduler_iterations = 0;

    struct cpu* cpu = mycpu();
    cpu->proc = 0;

    // Allows devices to interrupt.
    intr_on();

    for (int queue_index = 0; queue_index < QUEUE_COUNT; queue_index++) {
        for (struct proc* process = proc; process < &proc[NPROC]; process++) {
            acquire(&process->lock);

            if (process->state != RUNNABLE) {
                release(&process->lock);
                continue;
            }

            int* queue = queues[queue_index];
            int process_id = process->pid;

            int process_index;
            if (queue_index == HIGHEST_PRIORITY_QUEUE && !queues_contain(queues, process_id)) {
                process_index = try_add_process_to_queue(queue, process_id);

                if (process_index == -1) {
                    release(&process->lock);
                    continue;
                }
            } else {
                process_index = try_get_process_index_in_queue(queue, process_id);

                if (process_index == -1) {
                    release(&process->lock);
                    continue;
                }
            }

            // Switches to the process.
            process->state = RUNNING;
            cpu->proc = process;
            swtch(&cpu->context, &process->context);

            // Resets the CPU's process now that this process is done running.
            cpu->proc = 0;

            // Removes process from the queues if it ran to completion. Otherwise, lowers the
            // process's priority, unless it was already at the lowest priority.
            if (process->state != RUNNABLE) {
                remove_process_from_queue(queue, process_index);
            } else if (queue_index != LOWEST_PRIORITY_QUEUE) {
                int* lower_priority_queue = queues[queue_index + 1];

                if (try_add_process_to_queue(lower_priority_queue, process_id) != -1) {
                    remove_process_from_queue(queue, process_index);
                }
            }

            release(&process->lock);
        }
    }

    scheduler_iterations++;
    if (scheduler_iterations == SCHEDULER_ITERATION_PRIORITY_RESET_INTERVAL) {
        move_all_processes_to_highest_priority(queues);
        scheduler_iterations = 0;
    }
}

int queues_contain(int (*queues)[QUEUE_LENGTH], int process_id)
{
    for (int queue_index = 0; queue_index < QUEUE_COUNT; queue_index++) {
        int* queue = queues[queue_index];

        for (int process_index = 0; process_index < QUEUE_LENGTH; process_index++) {
            int other_process_id = queue[process_index];

            if (other_process_id != UNUSED_PROCESS && other_process_id == process_id) {
                return 1;
            }
        }
    }

    return 0;
};

int try_get_process_index_in_queue(int* queue, int process_id)
{
    for (int i = 0; i < QUEUE_LENGTH; i++) {
        int other_process_id = queue[i];

        if (other_process_id != UNUSED_PROCESS && other_process_id == process_id) {
            return i;
        }
    }

    return -1;
}

int try_add_process_to_queue(int* queue, int process_id)
{
    for (int i = 0; i < QUEUE_LENGTH; i++) {
        if (queue[i] == UNUSED_PROCESS) {
            queue[i] = process_id;
            return i;
        }
    }

    return -1;
};

void remove_process_from_queue(int* queue, int process_index)
{
    queue[process_index] = UNUSED_PROCESS;
}

void move_all_processes_to_highest_priority(int (*queues)[QUEUE_LENGTH])
{
    int* highest_priority_queue = queues[HIGHEST_PRIORITY_QUEUE];

    for (int queue_index = HIGHEST_PRIORITY_QUEUE + 1; queue_index < QUEUE_COUNT; queue_index++) {
        int* queue = queues[queue_index];

        for (int process_index = 0; process_index < QUEUE_LENGTH; queue_index++) {
            int process_id = queue[process_index];

            if (process_id != UNUSED_PROCESS) {
                remove_process_from_queue(queue, process_index);
                try_add_process_to_queue(highest_priority_queue, process_id);
            }
        }
    }
}
