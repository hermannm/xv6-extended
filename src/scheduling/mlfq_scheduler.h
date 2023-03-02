#ifndef MLFQ_SCHEDULER_H
#define MLFQ_SCHEDULER_H

#include "../../lib/kernel/param.h"

#define QUEUE_LENGTH NPROC

/**
 * Multi-Level Feedback Queue scheduler implementation.
 * Maintains a number of priority queues with process IDs. On each invocation, loops through every
 * process for each queue, and runs the processes in queue order.
 */
void mlfq_scheduler(void);

/**
 * Returns 1 if the given array of queues contains the given process ID, otherwise 0.
 */
int queues_contain(const int (*queues)[QUEUE_LENGTH], int process_id);

/**
 * Returns the index in the given queue where the given process ID is,
 * or -1 if it is not in the queue.
 */
int try_get_process_index_in_queue(const int* queue, int process_id);

/**
 * Inserts the given process ID in the first unused slot in the queue.
 * Returns the index where the process ID was inserted.
 * If the queue is full, returns -1.
 */
int try_add_process_to_queue(int* queue, int process_id);

/**
 * Sets the process ID slot at the given process index in the queue to unused.
 */
void remove_process_from_queue(int* queue, int process_index);

/**
 * Moves every process ID from lower-priority queues to the highest-priority queue.
 */
void move_all_processes_to_highest_priority(int (*queues)[QUEUE_LENGTH]);

#endif // MLFQ_SCHEDULER_H
