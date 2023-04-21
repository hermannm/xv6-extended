#ifndef USER_H
#define USER_H

#include "../kernel/types.h"

struct stat;
enum procstate {
    UNUSED,
    USED,
    SLEEPING,
    RUNNABLE,
    RUNNING,
    ZOMBIE,
    EXITED,
};

// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int *);
int pipe(int *);
int write(int, const void *, int);
int read(int, void *, int);
int close(int);
int kill(int);
int exec(const char *, char **);
int open(const char *, int);
int mknod(const char *, short, short);
int unlink(const char *);
int fstat(int fd, struct stat *);
int link(const char *, const char *);
int mkdir(const char *);
int chdir(const char *);
int dup(int);
int getpid(void);
char *sbrk(int);
int sleep(int);
int uptime(void);
uint64 schedls(void);
int schedset(int);
void pfreepages(void);
#define print_free_frame_cnt() (pfreepages())

// ulib.c
int stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, int);
char *strchr(const char *, char c);
int strcmp(const char *, const char *);
void fprintf(int, const char *, ...);
void printf(const char *, ...);
char *gets(char *, int max);
uint strlen(const char *);
void *memset(void *, int, uint);
void *malloc(uint);
void free(void *);
int atoi(const char *);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);

// uthread.c

/// @brief This is the thread context, storing the return address, the stack
///        pointer and all callee saved registers
struct thread_context {
    uint64 ra;
    uint64 sp;

    // callee-saved
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

/// @brief Switch from old thread context to new thread context.
/// @param old denotes the previous thread context, that will be switched out
/// @param new denotes the thread context of the thread that will be run next
extern void tswtch(struct thread_context *old, struct thread_context *new);

////////////////////////////////////////////////////////////////
/// NOTE: DON'T CHANGE THE FUNCTION OUTLINES BELOW THIS LINE ///
////////////////////////////////////////////////////////////////

// ulock.c
struct lock {
    uint8 locked;

    char *name;
    uint8 tid;
};

/// @brief Initializes lock struct. Needs to be called before using a lock for the first time
/// @param lk Pointer to the lock to be initialized
/// @param name Give your lock a name, which might help with debugging
void initlock(struct lock *lk, char *name);

/// @brief Checks if the current thread already holds the lock
/// @param lk The lock to be checked
/// @return Indicates whether lock is held or not
uint8 holding(struct lock *lk);

/// @brief Acquire a lock by yield-waiting on the lock
/// @param lk The lock to be acquired
void acquire(struct lock *lk);

/// @brief Release a lock
/// @param lk Lock to be released
void release(struct lock *lk);

#endif // USER_H
