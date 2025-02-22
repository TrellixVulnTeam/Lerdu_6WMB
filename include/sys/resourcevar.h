/*	$OpenBSD: resourcevar.h,v 1.14 2011/07/11 15:40:47 guenther Exp $	*/
/*	$NetBSD: resourcevar.h,v 1.12 1995/11/22 23:01:53 cgd Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)resourcevar.h	8.3 (Berkeley) 2/22/94
 */

#ifndef	_SYS_RESOURCEVAR_H_
#define	_SYS_RESOURCEVAR_H_

#include <sys/timeout.h>
#include <sys/resource.h>


/*
 * Kernel per-process accounting / statistics
 * (not necessarily resident except when running).
 * 内核中每个进程的统计（不一定常驻,除了运行时）。
 */
struct pstats {
#define	pstat_startzero	p_ru
	struct	rusage p_ru;		/* stats for this proc 统计为这个PROC */
	struct	rusage p_cru;		/* sum of stats for reaped children获得子的统计总和 */
	struct	itimerval p_timer[3];	/* virtual-time timers虚拟时间定时器 */
#define	pstat_endzero	pstat_startcopy

#define	pstat_startcopy	p_prof
	struct uprof {			/* profile arguments配置文件参数 */
		caddr_t	pr_base;	/* buffer base缓冲区的基 */
		size_t  pr_size;	/* buffer size缓冲区大小 */
		u_long	pr_off;		/* pc offset/PC偏移 */
		u_int   pr_scale;	/* pc scaling/PC缩放 */
		u_long	pr_addr;	/* temp storage for addr until AST/助攻临时存储地址 */
		u_long	pr_ticks;	/* temp storage for ticks until AST /助攻临时存储蜱*/
	} p_prof;
#define	pstat_endcopy	p_start
	struct	timeval p_start;	/* starting time开始时间 */
	struct	timeout p_virt_to;	/* virtual itimer虚拟定时器 trampoline. */
	struct	timeout p_prof_to;	/* prof itimer trampoline. */
};

/*
 * Kernel shareable process resource limits.  Because this structure
 * 内核共享进程的资源限制。因为这个结构
 * is moderately large but changes infrequently, it is shared
 * 中等大，但很少更改，它是共享
 * copy-on-write after forks.
 * 复本分支。
 */
struct plimit {
	struct	rlimit pl_rlimit[RLIM_NLIMITS];
	int	p_refcnt;		/* number of references 引用数*/
};

/* add user profiling from AST 从AST添加用户分析*/
#define	ADDUPROF(p)							\
do {									\
	atomic_clearbits_int(&(p)->p_flag, P_OWEUPC);			\
	addupc_task((p), (p)->p_stats->p_prof.pr_addr,			\
	    (p)->p_stats->p_prof.pr_ticks);				\
	(p)->p_stats->p_prof.pr_ticks = 0;				\
} while (0)

#ifdef _KERNEL
void	 addupc_intr(struct proc *p, u_long pc);
void	 addupc_task(struct proc *p, u_long pc, u_int ticks);
void	 calcru(struct proc *p, struct timeval *up, struct timeval *sp,
	    struct timeval *ip);
struct plimit *limcopy(struct plimit *lim);
void	limfree(struct plimit *);

void	 ruadd(struct rusage *ru, struct rusage *ru2);

void	virttimer_trampoline(void *);
void	proftimer_trampoline(void *);
#endif
#endif	/* !_SYS_RESOURCEVAR_H_ */
