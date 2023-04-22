#include "../kernel/types.h"
#include "user.h"

#include "../../src/threads/thread.h"
#include "../../src/threads/thread_list.h"

struct arg {
    int a;
    int b;
};
int shared_state = 0;
struct lock shared_state_lock;

// expected output: shared state = 2 in the end
void *race_for_state(void *arg)
{
    struct arg args = *(struct arg *)arg;
    printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
    if (shared_state == 0) {
        yield_thread();
        printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
        shared_state += args.a;
        printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
        yield_thread();
    } else {
        yield_thread();
        printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
        shared_state += args.b;
        printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
        yield_thread();
    }
    return 0;
}

// expected output: shared state = 3 in the end
void *no_race_for_state(void *arg)
{
    struct arg args = *(struct arg *)arg;
    printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
    acquire(&shared_state_lock);
    if (shared_state == 0) {
        yield_thread();
        printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
        shared_state += args.a;
        printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
        yield_thread();
    } else {
        yield_thread();
        printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
        shared_state += args.b;
        printf("%s[%d] %d\n", __FUNCTION__, get_current_thread_id(), shared_state);
        yield_thread();
    }
    release(&shared_state_lock);
    return 0;
}

void *print_hello_world(void *arg)
{
    printf("Hello World\n");
    return 0;
}

void *print_hello_world_with_tid(void *arg)
{
    printf("Hello World from Thread %d\n", get_current_thread_id());
    return 0;
}

void *calculate_rv(void *arg)
{
    struct arg args = *(struct arg *)arg;
    printf("child args: a=%d, b=%d\n", args.a, args.b);
    int *result = (int *)malloc(sizeof(int));
    *result = args.a + args.b;
    printf("child result: %d\n", *result);
    return (void *)result;
}

void test1()
{
    printf("[%s enter]\n", __FUNCTION__);
    struct thread *t;
    create_thread(&t, 0, &print_hello_world, 0);
    yield_thread();
    printf("[%s exit]\n", __FUNCTION__);
}

void test2()
{
    printf("[%s enter]\n", __FUNCTION__);
    struct thread *threadpool[8] = {0};
    for (int i = 0; i < 8; i++) {
        create_thread(&threadpool[i], 0, &print_hello_world_with_tid, 0);
    }
    for (int i = 0; i < 8; i++) {
        join_thread(threadpool[i]->id, 0, 0);
    }
    printf("[%s exit]\n", __FUNCTION__);
}

void test3()
{
    printf("[%s enter]\n", __FUNCTION__);
    struct thread *t;
    struct thread_attributes tattr;
    tattr.result_size = sizeof(int);
    tattr.stack_size = 512;
    struct arg args;
    args.a = 1;
    args.b = 10;
    create_thread(&t, &tattr, &calculate_rv, &args);
    int result;
    join_thread(t->id, &result, sizeof(int));
    printf("parent result: %d\n", result);
    printf("[%s exit]\n", __FUNCTION__);
}

void test4()
{
    printf("[%s enter]\n", __FUNCTION__);
    struct thread *ta;
    struct thread *tb;
    struct arg args;
    args.a = 1;
    args.b = 2;
    create_thread(&ta, 0, &race_for_state, &args);
    create_thread(&tb, 0, &race_for_state, &args);

    for (uint8 i = 0; i < 16; i++) {
        struct thread *thread = get_thread(i);
        printf("{ id: %d, state: %d }\n", thread->id, thread->state);
    }

    yield_thread();
    join_thread(ta->id, 0, 0);
    join_thread(tb->id, 0, 0);
    printf("[%s exit]\n", __FUNCTION__);
}

void test5()
{
    printf("[%s enter]\n", __FUNCTION__);
    initlock(&shared_state_lock, "sharedstate lock");
    struct thread *ta;
    struct thread *tb;
    struct arg args;
    args.a = 1;
    args.b = 2;
    create_thread(&ta, 0, &no_race_for_state, &args);
    create_thread(&tb, 0, &no_race_for_state, &args);
    yield_thread();
    join_thread(ta->id, 0, 0);
    join_thread(tb->id, 0, 0);
    printf("[%s exit]\n", __FUNCTION__);
}
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("ttest TEST_ID\n TEST ID\tId of the test to run. ID can be any value from 1 to 5\n");
        return -1;
    }

    switch (atoi(argv[1])) {
    case 1:
        test1();
        break;

    case 2:
        test2();
        break;

    case 3:
        test3();
        break;

    case 4:
        test4();
        break;

    case 5:
        test5();
        break;

    default:
        printf("Error: No test with index %s\n", argv[1]);
        return -1;
    }
    return 0;
    return 0;
}
