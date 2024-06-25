/*
NAME
pthread_mutex_lock, pthread_mutex_trylock, pthread_mutex_unlock − lock and unlock a mutex

SYNOPSIS
#include <pthread.h>

int pthread_mutex_lock(pthread_mutex_t *mutex); int pthread_mutex_trylock(pthread_mutex_t *mutex); int pthread_mutex_unlock(pthread_mutex_t *mutex);

DESCRIPTION
The mutex object referenced by mutex is locked by calling pthread_mutex_lock(). If the mutex is already locked, the calling thread blocks until the mutex becomes available. This operation returns with the mutex object referenced by mutex in the locked state with the calling thread as its owner.

If the mutex type is PTHREAD_MUTEX_NORMAL , deadlock detection is not provided. Attempting to relock the mutex causes deadlock. If a thread attempts to unlock a mutex that it has not locked or a mutex which is unlocked, undefined behaviour results.

If the mutex type is PTHREAD_MUTEX_ERRORCHECK , then error checking is provided. If a thread attempts to relock a mutex that it has already locked, an error will be returned. If a thread attempts to unlock a mutex that it has not locked or a mutex which is unlocked, an error will be returned.

If the mutex type is PTHREAD_MUTEX_RECURSIVE , then the mutex maintains the concept of a lock count. When a thread successfully acquires a mutex for the first time, the lock count is set to one. Every time a thread relocks this mutex, the lock count is incremented by one. Each time the thread unlocks the mutex, the lock count is decremented by one. When the lock count reaches zero, the mutex becomes available for other threads to acquire. If a thread attempts to unlock a mutex that it has not locked or a mutex which is unlocked, an error will be returned.

If the mutex type is PTHREAD_MUTEX_DEFAULT , attempting to recursively lock the mutex results in undefined behaviour. Attempting to unlock the mutex if it was not locked by the calling thread results in undefined behaviour. Attempting to unlock the mutex if it is not locked results in undefined behaviour.

The function pthread_mutex_trylock() is identical to pthread_mutex_lock() except that if the mutex object referenced by mutex is currently locked (by any thread, including the current thread), the call returns immediately.

The pthread_mutex_unlock() function releases the mutex object referenced by mutex. The manner in which a mutex is released is dependent upon the mutex’s type attribute. If there are threads blocked on the mutex object referenced by mutex when pthread_mutex_unlock() is called, resulting in the mutex becoming available, the scheduling policy is used to determine which thread shall acquire the mutex. (In the case of PTHREAD_MUTEX_RECURSIVE mutexes, the mutex becomes available when the count reaches zero and the calling thread no longer has any locks on this mutex).

If a signal is delivered to a thread waiting for a mutex, upon return from the signal handler the thread resumes waiting for the mutex as if it was not interrupted.

RETURN VALUE
If successful, the pthread_mutex_lock() and pthread_mutex_unlock() functions return zero. Otherwise, an error number is returned to indicate the error.

The function pthread_mutex_trylock() returns zero if a lock on the mutex object referenced by mutex is acquired. Otherwise, an error number is returned to indicate the error.

ERRORS
The pthread_mutex_lock() and pthread_mutex_trylock() functions will fail if:
[ EINVAL ]

The mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT and the calling thread’s priority is higher than the mutex’s current priority ceiling.

The pthread_mutex_trylock() function will fail if:
[ EBUSY ]

The mutex could not be acquired because it was already locked.

The pthread_mutex_lock(), pthread_mutex_trylock() and pthread_mutex_unlock() functions may fail if:
[ EINVAL ]

The value specified by mutex does not refer to an initialised mutex object.

[ EAGAIN ]

The mutex could not be acquired because the maximum number of recursive locks for mutex has been exceeded.

The pthread_mutex_lock() function may fail if:
[ EDEADLK ]

The current thread already owns the mutex.

The pthread_mutex_unlock() function may fail if:
[ EPERM ]

The current thread does not own the mutex.

These functions will not return an error code of [ EINTR ].

EXAMPLES
None.

APPLICATION USAGE
None.

FUTURE DIRECTIONS
None.

SEE ALSO
pthread_mutex_init(), pthread_mutex_destroy(), <pthread.h>.
*/