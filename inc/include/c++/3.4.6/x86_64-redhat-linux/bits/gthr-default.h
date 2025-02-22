/* Threads compatibility routines for libgcc2 and libobjc.  */
/* Compile this one with gcc.  */
/* Copyright (C) 1997, 1999, 2000, 2001, 2002, 2003
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

/* As a special exception, if you link this library with other files,
   some of which are compiled with GCC, to produce an executable,
   this library does not by itself cause the resulting executable
   to be covered by the GNU General Public License.
   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.  */

#ifndef _GLIBCXX_GCC_GTHR_POSIX_H
#define _GLIBCXX_GCC_GTHR_POSIX_H

/* POSIX threads specific definitions.
   Easy, since the interface is just one-to-one mapping.  */

#define __GTHREADS 1

/* Some implementations of <pthread.h> require this to be defined.  */
#if !defined(_REENTRANT) && defined(__osf__)
#define _REENTRANT 1
#endif

#include <pthread.h>
#include <unistd.h>

typedef pthread_key_t __gthread_key_t;
typedef pthread_once_t __gthread_once_t;
typedef pthread_mutex_t __gthread_mutex_t;
typedef pthread_mutex_t __gthread_recursive_mutex_t;

#define __GTHREAD_MUTEX_INIT PTHREAD_MUTEX_INITIALIZER
#define __GTHREAD_ONCE_INIT PTHREAD_ONCE_INIT
#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
#define __GTHREAD_RECURSIVE_MUTEX_INIT PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#elif defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
#define __GTHREAD_RECURSIVE_MUTEX_INIT PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#else
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION __gthread_recursive_mutex_init_function
#endif

#if __GXX_WEAK__ && _GLIBCXX_GTHREAD_USE_WEAK && defined __GNUC_RH_RELEASE__ \
    && ((__GNUC__ == 3 && __GNUC_MINOR__ == 4 && (__GNUC_PATCHLEVEL__ > 4 || (__GNUC_PATCHLEVEL__ == 4 && __GNUC_RH_RELEASE__ > 2))) \
	|| (__GNUC__ == 4 && __GNUC_MINOR__ == 0 && (__GNUC_PATCHLEVEL__ > 2 || (__GNUC_PATCHLEVEL__ == 2 && __GNUC_RH_RELEASE__ > 6)))) \
    && ! defined __attribute__
# define __gthrw(name) \
  extern __typeof(name) __gthrw_ ## name __attribute__ ((__weakref__(#name)))

__gthrw(pthread_once);
__gthrw(pthread_key_create);
__gthrw(pthread_key_delete);
__gthrw(pthread_getspecific);
__gthrw(pthread_setspecific);
__gthrw(pthread_create);

__gthrw(pthread_mutex_lock);
__gthrw(pthread_mutex_trylock);
__gthrw(pthread_mutex_unlock);
__gthrw(pthread_mutexattr_init);
__gthrw(pthread_mutexattr_settype);
__gthrw(pthread_mutexattr_destroy);

__gthrw(pthread_mutex_init);

# if defined(_LIBOBJC) || defined(_LIBOBJC_WEAK)
/* Objective-C.  */
__gthrw(pthread_cond_broadcast);
__gthrw(pthread_cond_destroy);
__gthrw(pthread_cond_init);
__gthrw(pthread_cond_signal);
__gthrw(pthread_cond_wait);
__gthrw(pthread_exit);
__gthrw(pthread_mutex_destroy);
__gthrw(pthread_self);
/* These really should be protected by _POSIX_PRIORITY_SCHEDULING, but
   we use them inside a _POSIX_THREAD_PRIORITY_SCHEDULING block.  */
#  ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
__gthrw(sched_get_priority_max);
__gthrw(sched_get_priority_min);
#  endif /* _POSIX_THREAD_PRIORITY_SCHEDULING */
__gthrw(sched_yield);
__gthrw(pthread_attr_destroy);
__gthrw(pthread_attr_init);
__gthrw(pthread_attr_setdetachstate);
#  ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
__gthrw(pthread_getschedparam);
__gthrw(pthread_setschedparam);
#  endif /* _POSIX_THREAD_PRIORITY_SCHEDULING */
# endif /* _LIBOBJC || _LIBOBJC_WEAK */
#else
# if __GXX_WEAK__ && _GLIBCXX_GTHREAD_USE_WEAK
#pragma weak pthread_once
#pragma weak pthread_key_create
#pragma weak pthread_key_delete
#pragma weak pthread_getspecific
#pragma weak pthread_setspecific
#pragma weak pthread_create

#pragma weak pthread_mutex_lock
#pragma weak pthread_mutex_trylock
#pragma weak pthread_mutex_unlock
#pragma weak pthread_mutexattr_init
#pragma weak pthread_mutexattr_settype
#pragma weak pthread_mutexattr_destroy

#pragma weak pthread_mutex_init

#if defined(_LIBOBJC) || defined(_LIBOBJC_WEAK)
/* Objective-C.  */
#pragma weak pthread_cond_broadcast
#pragma weak pthread_cond_destroy
#pragma weak pthread_cond_init
#pragma weak pthread_cond_signal
#pragma weak pthread_cond_wait
#pragma weak pthread_exit
#pragma weak pthread_mutex_destroy
#pragma weak pthread_self
/* These really should be protected by _POSIX_PRIORITY_SCHEDULING, but
   we use them inside a _POSIX_THREAD_PRIORITY_SCHEDULING block.  */
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
#pragma weak sched_get_priority_max
#pragma weak sched_get_priority_min
#endif /* _POSIX_THREAD_PRIORITY_SCHEDULING */
#pragma weak sched_yield
#pragma weak pthread_attr_destroy
#pragma weak pthread_attr_init
#pragma weak pthread_attr_setdetachstate
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
#pragma weak pthread_getschedparam
#pragma weak pthread_setschedparam
#endif /* _POSIX_THREAD_PRIORITY_SCHEDULING */
#endif /* _LIBOBJC || _LIBOBJC_WEAK */
#endif /* __GXX_WEAK__ && _GLIBCXX_GTHREAD_USE_WEAK */

#define __gthrw_pthread_once pthread_once
#define __gthrw_pthread_key_create pthread_key_create
#define __gthrw_pthread_key_delete pthread_key_delete
#define __gthrw_pthread_getspecific pthread_getspecific
#define __gthrw_pthread_setspecific pthread_setspecific
#define __gthrw_pthread_create pthread_create

#define __gthrw_pthread_mutex_lock pthread_mutex_lock
#define __gthrw_pthread_mutex_trylock pthread_mutex_trylock
#define __gthrw_pthread_mutex_unlock pthread_mutex_unlock
#define __gthrw_pthread_mutexattr_init pthread_mutexattr_init
#define __gthrw_pthread_mutexattr_settype pthread_mutexattr_settype
#define __gthrw_pthread_mutexattr_destroy pthread_mutexattr_destroy

#define __gthrw_pthread_mutex_init pthread_mutex_init

#if defined(_LIBOBJC) || defined(_LIBOBJC_WEAK)
/* Objective-C.  */
#define __gthrw_pthread_cond_broadcast pthread_cond_broadcast
#define __gthrw_pthread_cond_destroy pthread_cond_destroy
#define __gthrw_pthread_cond_init pthread_cond_init
#define __gthrw_pthread_cond_signal pthread_cond_signal
#define __gthrw_pthread_cond_wait pthread_cond_wait
#define __gthrw_pthread_exit pthread_exit
#define __gthrw_pthread_mutex_destroy pthread_mutex_destroy
#define __gthrw_pthread_self pthread_self
/* These really should be protected by _POSIX_PRIORITY_SCHEDULING, but
   we use them inside a _POSIX_THREAD_PRIORITY_SCHEDULING block.  */
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
#define __gthrw_sched_get_priority_max sched_get_priority_max
#define __gthrw_sched_get_priority_min sched_get_priority_min
#endif /* _POSIX_THREAD_PRIORITY_SCHEDULING */
#define __gthrw_sched_yield sched_yield
#define __gthrw_pthread_attr_destroy pthread_attr_destroy
#define __gthrw_pthread_attr_init pthread_attr_init
#define __gthrw_pthread_attr_setdetachstate pthread_attr_setdetachstate
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
#define __gthrw_pthread_getschedparam pthread_getschedparam
#define __gthrw_pthread_setschedparam pthread_setschedparam
#endif /* _POSIX_THREAD_PRIORITY_SCHEDULING */
#endif /* _LIBOBJC || _LIBOBJC_WEAK */
#endif


#if __GXX_WEAK__ && _GLIBCXX_GTHREAD_USE_WEAK

static inline int
__gthread_active_p (void)
{
  static void *const __gthread_active_ptr = (void *) &__gthrw_pthread_create;
  return __gthread_active_ptr != 0;
}

#else /* not __GXX_WEAK__ */

static inline int
__gthread_active_p (void)
{
  return 1;
}

#endif /* __GXX_WEAK__ */

#ifdef _LIBOBJC

/* This is the config.h file in libobjc/ */
#include <config.h>

#ifdef HAVE_SCHED_H
# include <sched.h>
#endif

/* Key structure for maintaining thread specific storage */
static pthread_key_t _objc_thread_storage;
static pthread_attr_t _objc_thread_attribs;

/* Thread local storage for a single thread */
static void *thread_local_storage = NULL;

/* Backend initialization functions */

/* Initialize the threads subsystem.  */
static inline int
__gthread_objc_init_thread_system (void)
{
  if (__gthread_active_p ())
    {
      /* Initialize the thread storage key */
      if (__gthrw_pthread_key_create (&_objc_thread_storage, NULL) == 0)
	{
	  /* The normal default detach state for threads is
	   * PTHREAD_CREATE_JOINABLE which causes threads to not die
	   * when you think they should.  */
	  if (__gthrw_pthread_attr_init (&_objc_thread_attribs) == 0
	      && __gthrw_pthread_attr_setdetachstate (&_objc_thread_attribs,
					      PTHREAD_CREATE_DETACHED) == 0)
	    return 0;
	}
    }

  return -1;
}

/* Close the threads subsystem.  */
static inline int
__gthread_objc_close_thread_system (void)
{
  if (__gthread_active_p ()
      && __gthrw_pthread_key_delete (_objc_thread_storage) == 0
      && __gthrw_pthread_attr_destroy (&_objc_thread_attribs) == 0)
    return 0;

  return -1;
}

/* Backend thread functions */

/* Create a new thread of execution.  */
static inline objc_thread_t
__gthread_objc_thread_detach (void (*func)(void *), void *arg)
{
  objc_thread_t thread_id;
  pthread_t new_thread_handle;

  if (!__gthread_active_p ())
    return NULL;

  if (!(__gthrw_pthread_create (&new_thread_handle, NULL, (void *) func, arg)))
    thread_id = (objc_thread_t) new_thread_handle;
  else
    thread_id = NULL;

  return thread_id;
}

/* Set the current thread's priority.  */
static inline int
__gthread_objc_thread_set_priority (int priority)
{
  if (!__gthread_active_p ())
    return -1;
  else
    {
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
      pthread_t thread_id = __gthrw_pthread_self ();
      int policy;
      struct sched_param params;
      int priority_min, priority_max;

      if (__gthrw_pthread_getschedparam (thread_id, &policy, &params) == 0)
	{
	  if ((priority_max = __gthrw_sched_get_priority_max (policy)) == -1)
	    return -1;

	  if ((priority_min = __gthrw_sched_get_priority_min (policy)) == -1)
	    return -1;

	  if (priority > priority_max)
	    priority = priority_max;
	  else if (priority < priority_min)
	    priority = priority_min;
	  params.sched_priority = priority;

	  /*
	   * The solaris 7 and several other man pages incorrectly state that
	   * this should be a pointer to policy but pthread.h is universally
	   * at odds with this.
	   */
	  if (__gthrw_pthread_setschedparam (thread_id, policy, &params) == 0)
	    return 0;
	}
#endif /* _POSIX_THREAD_PRIORITY_SCHEDULING */
      return -1;
    }
}

/* Return the current thread's priority.  */
static inline int
__gthread_objc_thread_get_priority (void)
{
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
  if (__gthread_active_p ())
    {
      int policy;
      struct sched_param params;

      if (__gthrw_pthread_getschedparam (__gthrw_pthread_self (), &policy, &params) == 0)
	return params.sched_priority;
      else
	return -1;
    }
  else
#endif /* _POSIX_THREAD_PRIORITY_SCHEDULING */
    return OBJC_THREAD_INTERACTIVE_PRIORITY;
}

/* Yield our process time to another thread.  */
static inline void
__gthread_objc_thread_yield (void)
{
  if (__gthread_active_p ())
    __gthrw_sched_yield ();
}

/* Terminate the current thread.  */
static inline int
__gthread_objc_thread_exit (void)
{
  if (__gthread_active_p ())
    /* exit the thread */
    __gthrw_pthread_exit (&__objc_thread_exit_status);

  /* Failed if we reached here */
  return -1;
}

/* Returns an integer value which uniquely describes a thread.  */
static inline objc_thread_t
__gthread_objc_thread_id (void)
{
  if (__gthread_active_p ())
    return (objc_thread_t) __gthrw_pthread_self ();
  else
    return (objc_thread_t) 1;
}

/* Sets the thread's local storage pointer.  */
static inline int
__gthread_objc_thread_set_data (void *value)
{
  if (__gthread_active_p ())
    return __gthrw_pthread_setspecific (_objc_thread_storage, value);
  else
    {
      thread_local_storage = value;
      return 0;
    }
}

/* Returns the thread's local storage pointer.  */
static inline void *
__gthread_objc_thread_get_data (void)
{
  if (__gthread_active_p ())
    return __gthrw_pthread_getspecific (_objc_thread_storage);
  else
    return thread_local_storage;
}

/* Backend mutex functions */

/* Allocate a mutex.  */
static inline int
__gthread_objc_mutex_allocate (objc_mutex_t mutex)
{
  if (__gthread_active_p ())
    {
      mutex->backend = objc_malloc (sizeof (pthread_mutex_t));

      if (__gthrw_pthread_mutex_init ((pthread_mutex_t *) mutex->backend, NULL))
	{
	  objc_free (mutex->backend);
	  mutex->backend = NULL;
	  return -1;
	}
    }

  return 0;
}

/* Deallocate a mutex.  */
static inline int
__gthread_objc_mutex_deallocate (objc_mutex_t mutex)
{
  if (__gthread_active_p ())
    {
      int count;

      /*
       * Posix Threads specifically require that the thread be unlocked
       * for __gthrw_pthread_mutex_destroy to work.
       */

      do
	{
	  count = __gthrw_pthread_mutex_unlock ((pthread_mutex_t *) mutex->backend);
	  if (count < 0)
	    return -1;
	}
      while (count);

      if (__gthrw_pthread_mutex_destroy ((pthread_mutex_t *) mutex->backend))
	return -1;

      objc_free (mutex->backend);
      mutex->backend = NULL;
    }
  return 0;
}

/* Grab a lock on a mutex.  */
static inline int
__gthread_objc_mutex_lock (objc_mutex_t mutex)
{
  if (__gthread_active_p ()
      && __gthrw_pthread_mutex_lock ((pthread_mutex_t *) mutex->backend) != 0)
    {
      return -1;
    }

  return 0;
}

/* Try to grab a lock on a mutex.  */
static inline int
__gthread_objc_mutex_trylock (objc_mutex_t mutex)
{
  if (__gthread_active_p ()
      && __gthrw_pthread_mutex_trylock ((pthread_mutex_t *) mutex->backend) != 0)
    {
      return -1;
    }

  return 0;
}

/* Unlock the mutex */
static inline int
__gthread_objc_mutex_unlock (objc_mutex_t mutex)
{
  if (__gthread_active_p ()
      && __gthrw_pthread_mutex_unlock ((pthread_mutex_t *) mutex->backend) != 0)
    {
      return -1;
    }

  return 0;
}

/* Backend condition mutex functions */

/* Allocate a condition.  */
static inline int
__gthread_objc_condition_allocate (objc_condition_t condition)
{
  if (__gthread_active_p ())
    {
      condition->backend = objc_malloc (sizeof (pthread_cond_t));

      if (__gthrw_pthread_cond_init ((pthread_cond_t *) condition->backend, NULL))
	{
	  objc_free (condition->backend);
	  condition->backend = NULL;
	  return -1;
	}
    }

  return 0;
}

/* Deallocate a condition.  */
static inline int
__gthread_objc_condition_deallocate (objc_condition_t condition)
{
  if (__gthread_active_p ())
    {
      if (__gthrw_pthread_cond_destroy ((pthread_cond_t *) condition->backend))
	return -1;

      objc_free (condition->backend);
      condition->backend = NULL;
    }
  return 0;
}

/* Wait on the condition */
static inline int
__gthread_objc_condition_wait (objc_condition_t condition, objc_mutex_t mutex)
{
  if (__gthread_active_p ())
    return __gthrw_pthread_cond_wait ((pthread_cond_t *) condition->backend,
			      (pthread_mutex_t *) mutex->backend);
  else
    return 0;
}

/* Wake up all threads waiting on this condition.  */
static inline int
__gthread_objc_condition_broadcast (objc_condition_t condition)
{
  if (__gthread_active_p ())
    return __gthrw_pthread_cond_broadcast ((pthread_cond_t *) condition->backend);
  else
    return 0;
}

/* Wake up one thread waiting on this condition.  */
static inline int
__gthread_objc_condition_signal (objc_condition_t condition)
{
  if (__gthread_active_p ())
    return __gthrw_pthread_cond_signal ((pthread_cond_t *) condition->backend);
  else
    return 0;
}

#else /* _LIBOBJC */

static inline int
__gthread_once (__gthread_once_t *once, void (*func) (void))
{
  if (__gthread_active_p ())
    return __gthrw_pthread_once (once, func);
  else
    return -1;
}

static inline int
__gthread_key_create (__gthread_key_t *key, void (*dtor) (void *))
{
  return __gthrw_pthread_key_create (key, dtor);
}

static inline int
__gthread_key_delete (__gthread_key_t key)
{
  return __gthrw_pthread_key_delete (key);
}

static inline void *
__gthread_getspecific (__gthread_key_t key)
{
  return __gthrw_pthread_getspecific (key);
}

static inline int
__gthread_setspecific (__gthread_key_t key, const void *ptr)
{
  return __gthrw_pthread_setspecific (key, ptr);
}

static inline int
__gthread_mutex_lock (__gthread_mutex_t *mutex)
{
  if (__gthread_active_p ())
    return __gthrw_pthread_mutex_lock (mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_trylock (__gthread_mutex_t *mutex)
{
  if (__gthread_active_p ())
    return __gthrw_pthread_mutex_trylock (mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_unlock (__gthread_mutex_t *mutex)
{
  if (__gthread_active_p ())
    return __gthrw_pthread_mutex_unlock (mutex);
  else
    return 0;
}

#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
static inline int
__gthread_recursive_mutex_init_function (__gthread_recursive_mutex_t *mutex)
{
  if (__gthread_active_p ())
    {
      pthread_mutexattr_t attr;
      int r;

      r = __gthrw_pthread_mutexattr_init (&attr);
      if (!r)
	r = __gthrw_pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
      if (!r)
	r = __gthrw_pthread_mutex_init (mutex, &attr);
      if (!r)
	r = __gthrw_pthread_mutexattr_destroy (&attr);
      return r;
    }
}
#endif

static inline int
__gthread_recursive_mutex_lock (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_mutex_lock (mutex);
}

static inline int
__gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_mutex_trylock (mutex);
}

static inline int
__gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_mutex_unlock (mutex);
}

#endif /* _LIBOBJC */

#endif /* ! _GLIBCXX_GCC_GTHR_POSIX_H */
