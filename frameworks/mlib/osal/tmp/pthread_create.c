/*
pthread_create

NAME
       pthread_create - create a new thread

SYNOPSIS
       #include <pthread.h>

       int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);

       Compile and link with -pthread.

DESCRIPTION
       The pthread_create() function starts a new thread in the calling process.  The new thread starts execution by invoking start_routine(); arg is passed as the sole argument of start_routine().

       The new thread terminates in one of the following ways:

       * It calls pthread_exit(3), specifying an exit status value that is available to another thread in the same process that calls pthread_join(3).

       * It returns from start_routine().  This is equivalent to calling pthread_exit(3) with the value supplied in the return statement.

       * It is canceled (see pthread_cancel(3)).

       * Any of the threads in the process calls exit(3), or the main thread performs a return from main().  This causes the termination of all threads in the process.

       The attr argument points to a pthread_attr_t structure whose contents are used at thread creation time to determine attributes for the new thread; this structure is initialized using pthread_attr_init(3) and related functions.  If attr is
       NULL, then the thread is created with default attributes.

       Before returning, a successful call to pthread_create() stores the ID of the new thread in the buffer pointed to by thread; this identifier is used to refer to the thread in subsequent calls to other pthreads functions.

       The new thread inherits a copy of the creating thread's signal mask (pthread_sigmask(3)).  The set of pending signals for the new thread is empty (sigpending(2)).  The new thread does not inherit the  creating  thread's  alternate  signal
       stack (sigaltstack(2)).

       The new thread inherits the calling thread's floating-point environment (fenv(3)).

       The initial value of the new thread's CPU-time clock is 0 (see pthread_getcpuclockid(3)).

   Linux-specific details
       The new thread inherits copies of the calling thread's capability sets (see capabilities(7)) and CPU affinity mask (see sched_setaffinity(2)).

RETURN VALUE
       On success, pthread_create() returns 0; on error, it returns an error number, and the contents of *thread are undefined.

ERRORS
       EAGAIN Insufficient resources to create another thread.

       EAGAIN A  system-imposed  limit  on  the  number  of threads was encountered.  There are a number of limits that may trigger this error: the RLIMIT_NPROC soft resource limit (set via setrlimit(2)), which limits the number of processes and
              threads for a real user ID, was reached; the kernel's system-wide limit on the number of processes and threads, /proc/sys/kernel/threads-max, was reached (see proc(5)); or the maximum number of PIDs,  /proc/sys/kernel/pid_max,  was
              reached (see proc(5)).

       EINVAL Invalid settings in attr.

       EPERM  No permission to set the scheduling policy and parameters specified in attr.
*/