/*
NAME
pthread_mutex_init, pthread_mutex_destroy − initialise or destroy a mutex

SYNOPSIS
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr); int pthread_mutex_destroy(pthread_mutex_t *mutex); pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER ;

DESCRIPTION
The pthread_mutex_init() function initialises the mutex referenced by mutex with attributes specified by attr. If attr is NULL , the default mutex attributes are used; the effect is the same as passing the address of a default mutex attributes object. Upon successful initialisation, the state of the mutex becomes initialised and unlocked.

Attempting to initialise an already initialised mutex results in undefined behaviour.

The pthread_mutex_destroy() function destroys the mutex object referenced by mutex; the mutex object becomes, in effect, uninitialised. An implementation may cause pthread_mutex_destroy() to set the object referenced by mutex to an invalid value. A destroyed mutex object can be re-initialised using pthread_mutex_init(); the results of otherwise referencing the object after it has been destroyed are undefined.

It is safe to destroy an initialised mutex that is unlocked. Attempting to destroy a locked mutex results in undefined behaviour.

In cases where default mutex attributes are appropriate, the macro PTHREAD_MUTEX_INITIALIZER can be used to initialise mutexes that are statically allocated. The effect is equivalent to dynamic initialisation by a call to pthread_mutex_init() with parameter attr specified as NULL , except that no error checks are performed.

RETURN VALUE
If successful, the pthread_mutex_init() and pthread_mutex_destroy() functions return zero. Otherwise, an error number is returned to indicate the error. The [ EBUSY ] and [ EINVAL ] error checks, if implemented, act as if they were performed immediately at the beginning of processing for the function and cause an error return prior to modifying the state of the mutex specified by mutex.

ERRORS
The pthread_mutex_init() function will fail if:
[ EAGAIN ]

The system lacked the necessary resources (other than memory) to initialise another mutex.

[ ENOMEM ]

Insufficient memory exists to initialise the mutex.

[ EPERM ]

The caller does not have the privilege to perform the operation.

The pthread_mutex_init() function may fail if:
[ EBUSY ]

The implementation has detected an attempt to re-initialise the object referenced by mutex, a previously initialised, but not yet destroyed, mutex.

[ EINVAL ]

The value specified by attr is invalid.

The pthread_mutex_destroy() function may fail if:
[ EBUSY ]

The implementation has detected an attempt to destroy the object referenced by mutex while it is locked or referenced (for example, while being used in a pthread_cond_wait() or pthread_cond_timedwait()) by another thread.

[ EINVAL ]

The value specified by mutex is invalid.

These functions will not return an error code of [ EINTR ].




NAME
       pthread_mutex_destroy, pthread_mutex_init — destroy and initialize a mutex

SYNOPSIS
       #include <pthread.h>

       int pthread_mutex_destroy(pthread_mutex_t *mutex);
       int pthread_mutex_init(pthread_mutex_t *restrict mutex,
           const pthread_mutexattr_t *restrict attr);
       pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

DESCRIPTION
       The pthread_mutex_destroy() function shall destroy the mutex object referenced by mutex; the mutex object becomes, in effect, uninitialized. An implementation may cause pthread_mutex_destroy() to set the object referenced by mutex  to  an
       invalid value.

       A destroyed mutex object can be reinitialized using pthread_mutex_init(); the results of otherwise referencing the object after it has been destroyed are undefined.

       It  shall be safe to destroy an initialized mutex that is unlocked.  Attempting to destroy a locked mutex or a mutex that is referenced (for example, while being used in a pthread_cond_timedwait() or pthread_cond_wait()) by another thread
       results in undefined behavior.

       The pthread_mutex_init() function shall initialize the mutex referenced by mutex with attributes specified by attr.  If attr is NULL, the default mutex attributes are used; the effect shall be the same as passing the address of a  default
       mutex attributes object. Upon successful initialization, the state of the mutex becomes initialized and unlocked.

       Only mutex itself may be used for performing synchronization. The result of referring to copies of mutex in calls to pthread_mutex_lock(), pthread_mutex_trylock(), pthread_mutex_unlock(), and pthread_mutex_destroy() is undefined.

       Attempting to initialize an already initialized mutex results in undefined behavior.

       In cases where default mutex attributes are appropriate, the macro PTHREAD_MUTEX_INITIALIZER can be used to initialize mutexes. The effect shall be equivalent to dynamic initialization by a call to pthread_mutex_init() with parameter attr
       specified as NULL, except that no error checks are performed.

       The behavior is undefined if the value specified by the mutex argument to pthread_mutex_destroy() does not refer to an initialized mutex.

       The behavior is undefined if the value specified by the attr argument to pthread_mutex_init() does not refer to an initialized mutex attributes object.

RETURN VALUE
       If successful, the pthread_mutex_destroy() and pthread_mutex_init() functions shall return zero; otherwise, an error number shall be returned to indicate the error.

ERRORS
       The pthread_mutex_init() function shall fail if:

       EAGAIN The system lacked the necessary resources (other than memory) to initialize another mutex.

       ENOMEM Insufficient memory exists to initialize the mutex.

       EPERM  The caller does not have the privilege to perform the operation.

       The pthread_mutex_init() function may fail if:

       EINVAL The attributes object referenced by attr has the robust mutex attribute set without the process-shared attribute being set.

       These functions shall not return an error code of [EINTR].

       The following sections are informative.
*/