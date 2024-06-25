/*
NAME
       sem_init - initialize an unnamed semaphore

SYNOPSIS
       #include <semaphore.h>

       int sem_init(sem_t *sem, int pshared, unsigned int value);

       Link with -pthread.

DESCRIPTION
       sem_init() initializes the unnamed semaphore at the address pointed to by sem.  The value argument specifies the initial value for the semaphore.

       The pshared argument indicates whether this semaphore is to be shared between the threads of a process, or between processes.

       If pshared has the value 0, then the semaphore is shared between the threads of a process, and should be located at some address that is visible to all threads (e.g., a global variable, or a variable allocated dynamically on the heap).

       If pshared is nonzero, then the semaphore is shared between processes, and should be located in a region of shared memory (see shm_open(3), mmap(2), and shmget(2)).  (Since a child created by fork(2) inherits its parent's memory mappings,
       it can also access the semaphore.)  Any process that can access the shared memory region can operate on the semaphore using sem_post(3), sem_wait(3), and so on.

       Initializing a semaphore that has already been initialized results in undefined behavior.

RETURN VALUE
       sem_init() returns 0 on success; on error, -1 is returned, and errno is set to indicate the error.

ERRORS
       EINVAL value exceeds SEM_VALUE_MAX.

       ENOSYS pshared is nonzero, but the system does not support process-shared semaphores (see sem_overview(7)).

NAME
       sem_destroy - destroy an unnamed semaphore

SYNOPSIS
       #include <semaphore.h>

       int sem_destroy(sem_t *sem);

       Link with -pthread.

DESCRIPTION
       sem_destroy() destroys the unnamed semaphore at the address pointed to by sem.

       Only a semaphore that has been initialized by sem_init(3) should be destroyed using sem_destroy().

       Destroying a semaphore that other processes or threads are currently blocked on (in sem_wait(3)) produces undefined behavior.

       Using a semaphore that has been destroyed produces undefined results, until the semaphore has been reinitialized using sem_init(3).

RETURN VALUE
       sem_destroy() returns 0 on success; on error, -1 is returned, and errno is set to indicate the error.

ERRORS
       EINVAL sem is not a valid semaphore.
*/