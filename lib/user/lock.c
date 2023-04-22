// Similar to the kernel spinlock but for threads in userspace
#include "../kernel/types.h"
#include "user.h"

#include "../../src/threads/thread.h"

void initlock(struct lock *lk, char *name)
{
    lk->name = name;
    lk->locked = 0;
    lk->tid = -1;
}

uint8 holding(struct lock *lk)
{
    return lk->locked && lk->tid == get_current_thread_id();
}

void acquire(struct lock *lk)
{
    if (holding(lk)) {
        printf("re-acquiring lock we already hold");
        exit(-1);
    }

    while (__sync_lock_test_and_set(&lk->locked, 1) != 0) {
        // give up the cpu for other threads
        yield_thread();
    }

    __sync_synchronize();

    lk->tid = get_current_thread_id();
}

void release(struct lock *lk)
{
    if (!holding(lk)) {
        printf("releasing lock we are not holding");
        exit(-1);
    }

    lk->tid = -1;
    __sync_synchronize();
    __sync_lock_release(&lk->locked);
    yield_thread(); // yield that other threads that need the lock can grab it
}
