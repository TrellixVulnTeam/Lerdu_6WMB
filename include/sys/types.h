/*	$OpenBSD: types.h,v 1.32 2011/03/19 18:26:06 deraadt Exp $	*/
/*	$NetBSD: types.h,v 1.29 1996/11/15 22:48:25 jtc Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Copyright (c) 1982, 1986, 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.4 (Berkeley) 1/21/94
 */

#ifndef _SYS_TYPES_H_
#define	_SYS_TYPES_H_

#include <sys/cdefs.h>
#include <sys/_types.h>


#ifndef KERNEL
typedef unsigned long	object_t;
typedef unsigned long	task_t;
typedef unsigned long	thread_t;
typedef unsigned long	mutex_t;
typedef unsigned long	cond_t;
typedef unsigned long	sem_t;
typedef unsigned long	device_t;
#else
typedef struct object	*object_t;
typedef struct task	*task_t;
typedef struct thread	*thread_t;
typedef struct device	*device_t;
typedef struct mutex	*mutex_t;
typedef struct cond	*cond_t;
typedef struct sem	*sem_t;
typedef struct vm_map	*vm_map_t;
typedef struct irq	*irq_t;
#endif /* KERNEL */

#define OBJECT_NULL	((object_t)0)
#define TASK_NULL	  ((task_t)0)
#define THREAD_NULL	((thread_t)0)
#define MUTEX_NULL	 ((mutex_t)0)
#define COND_NULL	  ((cond_t)0)
#define SEM_NULL	   ((sem_t)0)
#define NODEV		((device_t)0)
#ifdef KERNEL
typedef void (*dkifn_t)(void);
#endif

//#define	NBBY	8		/* number of bits in a byte */

#ifndef KERNEL
/*
 * Select uses bit masks of file descriptors in longs.  These macros
 * manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	16
#endif

#ifndef howmany
#define	howmany(x, y)	(((x) + ((y) - 1)) / (y))
#endif

/*typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define	FD_COPY(f, t)	bcopy(f, t, sizeof(*(f)))
#define	FD_ZERO(p)	bzero(p, sizeof(*(p)))*/
#endif /* !KERNEL */

#if __BSD_VISIBLE
typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;

typedef unsigned char	unchar;		/* Sys V compatibility */
typedef	unsigned short	ushort;		/* Sys V compatibility */
typedef	unsigned int	uint;		/* Sys V compatibility */
typedef unsigned long	ulong;		/* Sys V compatibility */

typedef	__cpuid_t	cpuid_t;	/* CPU id */
typedef	__register_t	register_t;	/* register-sized type */
#endif /* __BSD_VISIBLE */

/*
 * XXX The exact-width bit types should only be exposed if __BSD_VISIBLE
 *     but the rest of the includes are not ready for that yet.
 */
#ifndef	__BIT_TYPES_DEFINED__
#define	__BIT_TYPES_DEFINED__
#endif

#ifndef	_INT8_T_DEFINED_
#define	_INT8_T_DEFINED_
typedef	__int8_t		int8_t;
#endif

#ifndef	_UINT8_T_DEFINED_
#define	_UINT8_T_DEFINED_
typedef	__uint8_t		uint8_t;
#endif

#ifndef	_INT16_T_DEFINED_
#define	_INT16_T_DEFINED_
typedef	__int16_t		int16_t;
#endif

#ifndef	_UINT16_T_DEFINED_
#define	_UINT16_T_DEFINED_
typedef	__uint16_t		uint16_t;
#endif

#ifndef	_INT32_T_DEFINED_
#define	_INT32_T_DEFINED_
typedef	__int32_t		int32_t;
#endif

#ifndef	_UINT32_T_DEFINED_
#define	_UINT32_T_DEFINED_
typedef	__uint32_t		uint32_t;
#endif

#ifndef	_INT64_T_DEFINED_
#define	_INT64_T_DEFINED_
typedef	__int64_t		int64_t;
#endif

#ifndef	_UINT64_T_DEFINED_
#define	_UINT64_T_DEFINED_
typedef	__uint64_t		uint64_t;
#endif

/* BSD-style unsigned bits types */
typedef	__uint8_t	u_int8_t;
typedef	__uint16_t	u_int16_t;
typedef	__uint32_t	u_int32_t;
typedef	__uint64_t	u_int64_t;

/* quads, deprecated in favor of 64 bit int types */
typedef	__int64_t	quad_t;
typedef	__uint64_t	u_quad_t;
typedef	quad_t *	qaddr_t;

#if __BSD_VISIBLE
/* VM system types */
typedef __vaddr_t	vaddr_t;
typedef __paddr_t	paddr_t;
typedef __vsize_t	vsize_t;
typedef __psize_t	psize_t;
#endif /* __BSD_VISIBLE */

/* Standard system types */
typedef	char *		caddr_t;	/* core address */
typedef	__int32_t	daddr32_t;	/* 32-bit disk address */
typedef	__int64_t	daddr_t;	/* 64-bit disk address */
typedef	__int64_t	daddr64_t;	/* 64-bit disk address */
typedef	__dev_t		dev_t;		/* device number */
typedef	__fixpt_t	fixpt_t;	/* fixed point number */
typedef	__gid_t		gid_t;		/* group id */
typedef	__id_t		id_t;		/* may contain pid, uid or gid */
typedef	__ino_t		ino_t;		/* inode number */
typedef	__key_t		key_t;		/* IPC key (for Sys V IPC) */
typedef	__mode_t	mode_t;		/* permissions */
typedef	__nlink_t	nlink_t;	/* link count */
typedef	__pid_t		pid_t;		/* process id */
typedef __rlim_t	rlim_t;		/* resource limit */
typedef	__segsz_t	segsz_t;	/* segment size */
typedef	__swblk_t	swblk_t;	/* swap offset */
typedef	__uid_t		uid_t;		/* user id */
typedef	__useconds_t	useconds_t;	/* microseconds */
typedef	__suseconds_t	suseconds_t;	/* microseconds (signed) */
typedef	__fsblkcnt_t	fsblkcnt_t;	/* file system block count */
typedef	__fsfilcnt_t	fsfilcnt_t;	/* file system file count */

/*
 * XPG4.2 states that inclusion of <netinet/in.h> must pull these
 * in and that inclusion of <sys/socket.h> must pull in sa_family_t.
 * We put these here because there are other headers that require
 * these types and <sys/socket.h> and <netinet/in.h> will indirectly
 * include <sys/types.h>.
 * XXX - now that we have protected versions these should move.
 */
typedef __in_addr_t	in_addr_t;	/* base type for internet address */
typedef __in_port_t	in_port_t;	/* IP port type */
typedef __sa_family_t	sa_family_t;	/* sockaddr address family type */
typedef __socklen_t	socklen_t;	/* length type for network syscalls */

/*
 * The following types may be defined in multiple header files.
 */
#ifndef	_CLOCK_T_DEFINED_
#define	_CLOCK_T_DEFINED_
typedef	__clock_t	clock_t;
#endif

#ifndef	_CLOCKID_T_DEFINED_
#define	_CLOCKID_T_DEFINED_
typedef	__clockid_t	clockid_t;
#endif

#ifndef	_SIZE_T_DEFINED_
#define	_SIZE_T_DEFINED_
typedef	__size_t	size_t;
#endif

#ifndef	_SSIZE_T_DEFINED_
#define	_SSIZE_T_DEFINED_
typedef	__ssize_t	ssize_t;
#endif

#ifndef	_TIME_T_DEFINED_
#define	_TIME_T_DEFINED_
typedef	__time_t	time_t;
#endif

#ifndef	_TIMER_T_DEFINED_
#define	_TIMER_T_DEFINED_
typedef	__timer_t	timer_t;
#endif

#ifndef	_OFF_T_DEFINED_
#define	_OFF_T_DEFINED_
typedef	__off_t		off_t;
#endif

/*
 * These belong in unistd.h, but are placed here too to ensure that
 * long arguments will be promoted to off_t if the program fails to
 * include that header or explicitly cast them to off_t.
 */
#if __BSD_VISIBLE && !defined(_KERNEL)
__BEGIN_DECLS
off_t	 lseek(int, off_t, int);
int	 ftruncate(int, off_t);
int	 truncate(const char *, off_t);
__END_DECLS
#endif /* __BSD_VISIBLE && !_KERNEL */

#if __BSD_VISIBLE
/* Major, minor numbers, dev_t's. */
#define	major(x)	((int32_t)(((u_int32_t)(x) >> 8) & 0xff))
#define	minor(x)	((int32_t)((x) & 0xff) | (((x) & 0xffff0000) >> 8))
#define	makedev(x,y)	((dev_t)((((x) & 0xff) << 8) | ((y) & 0xff) | (((y) & 0xffff00) << 8)))
#endif

#if __BSD_VISIBLE
#include <sys/select.h>	/* must be after type declarations */
#endif

#if defined(__STDC__) && defined(_KERNEL)
/*
 * Forward structure declarations for function prototypes.  We include the
 * common structures that cross subsystem boundaries here; others are mostly
 * used in the same place that the structure is defined.
 */
struct	proc;
struct	pgrp;
struct	ucred;
struct	rusage;
struct	file;
struct	buf;
struct	tty;
struct	uio;
#endif

#endif /* !_SYS_TYPES_H_ */
