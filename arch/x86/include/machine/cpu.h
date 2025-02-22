/*	$OpenBSD: cpu.h,v 1.30 2011/11/16 20:50:18 deraadt Exp $	*/
/*	$NetBSD: cpu.h,v 1.34 2003/06/23 11:01:08 martin Exp $	*/

/*
 * Copyright (c) 1994-1996 Mark Brinicombe.
 * Copyright (c) 1994 Brini.
 * All rights reserved.
 *
 * This code is derived from software written for Brini by Mark Brinicombe
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Brini.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BRINI ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL BRINI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * RiscBSD kernel project
 *
 * cpu.h
 *
 * CPU specific symbols
 *
 * Created      : 18/09/94
 *
 * Based on kate/katelib/arm6.h
 */

#include <sys/types.h>

#ifndef _ARM_CPU_H_
#define _ARM_CPU_H_

/*
 * User-visible definitions
 */

/*  CTL_MACHDEP definitions. */
		/*		1	   formerly int: CPU_DEBUG */
		/*		2	   formerly string: CPU_BOOTED_DEVICE */
		/*		3	   formerly string: CPU_BOOTED_KERNEL */
#define	CPU_CONSDEV		4	/* struct: dev_t of our console */
#define	CPU_POWERSAVE		5	/* int: use CPU powersave mode */
#define	CPU_ALLOWAPERTURE	6	/* int: allow mmap of /dev/xf86 */
#define CPU_APMWARN		7	/* APM battery warning percentage */
		/*		8	   formerly int: keyboard reset */
		/*		9	   formerly int: CPU_ZTSRAWMODE */
		/*		10	   formerly struct: CPU_ZTSSCALE */
#define	CPU_MAXSPEED		11	/* int: number of valid machdep ids */
#define CPU_LIDSUSPEND		12	/* int: closing lid causes suspend */
#define	CPU_MAXID		13	/* number of valid machdep ids */

#define	CTL_MACHDEP_NAMES { \
	{ 0, 0 }, \
	{ 0, 0 }, \
	{ 0, 0 }, \
	{ 0, 0 }, \
	{ "console_device", CTLTYPE_STRUCT }, \
	{ "powersave", CTLTYPE_INT }, \
	{ "allowaperture", CTLTYPE_INT }, \
	{ "apmwarn", CTLTYPE_INT }, \
	{ 0, 0 }, \
	{ 0, 0 }, \
	{ 0, 0 }, \
	{ "maxspeed", CTLTYPE_INT }, \
	{ "lidsuspend", CTLTYPE_INT } \
}

#ifdef _KERNEL

/*
 * Kernel-only definitions
 */

//#include <arm/cpuconf.h>

//lerdu-->#include <machine/intr.h>
#ifndef _LOCORE
#if 0
#include <sys/user.h>
#endif
//lerdu-->#include <machine/frame.h>
//lerdu-->#include <machine/pcb.h>
#endif	/* !_LOCORE */

//#include <arm/armreg.h>

#ifndef _LOCORE
/* 1 == use cpu_sleep(), 0 == don't */
extern int cpu_do_powersave;
#endif

#ifdef _LOCORE
#define IRQdisable \
	stmfd	sp!, {r0} ; \
	mrs	r0, cpsr ; \
	orr	r0, r0, #(I32_bit) ; \
	msr	cpsr_c, r0 ; \
	ldmfd	sp!, {r0}

#define IRQenable \
	stmfd	sp!, {r0} ; \
	mrs	r0, cpsr ; \
	bic	r0, r0, #(I32_bit) ; \
	msr	cpsr_c, r0 ; \
	ldmfd	sp!, {r0}		

#else
#define IRQdisable __set_cpsr_c(I32_bit, I32_bit);
#define IRQenable __set_cpsr_c(I32_bit, 0);
#endif	/* _LOCORE */

/* All the CLKF_* macros take a struct clockframe * as an argument. */

/*
 * CLKF_USERMODE: Return TRUE/FALSE (1/0) depending on whether the
 * frame came from USR mode or not.
 */
#define CLKF_USERMODE(frame)	((frame->if_spsr & PSR_MODE) == PSR_USR32_MODE)

/*
 * CLKF_INTR: True if we took the interrupt from inside another
 * interrupt handler.
 */
extern int current_intr_depth;
#define CLKF_INTR(frame)	(current_intr_depth > 1) 

/*
 * CLKF_PC: Extract the program counter from a clockframe
 */
#define CLKF_PC(frame)		(frame->if_pc)

/*
 * PROC_PC: Find out the program counter for the given process.
 */
#define PROC_PC(p)	((p)->p_addr->u_pcb.pcb_tf->tf_pc)

/* The address of the vector page. */
extern vaddr_t vector_page;
void	arm32_vector_init(vaddr_t, int);

#define	ARM_VEC_RESET			(1 << 0)
#define	ARM_VEC_UNDEFINED		(1 << 1)
#define	ARM_VEC_SWI			(1 << 2)
#define	ARM_VEC_PREFETCH_ABORT		(1 << 3)
#define	ARM_VEC_DATA_ABORT		(1 << 4)
#define	ARM_VEC_ADDRESS_EXCEPTION	(1 << 5)
#define	ARM_VEC_IRQ			(1 << 6)
#define	ARM_VEC_FIQ			(1 << 7)

#define	ARM_NVEC			8
#define	ARM_VEC_ALL			0xffffffff

/*
 * Per-CPU information.  For now we assume one CPU.
 */

#include <sys/device.h>
#include <sys/sched.h>

struct cpu_info {
	struct proc *ci_curproc;

	struct schedstate_percpu ci_schedstate; /* scheduler state */
#if defined(DIAGNOSTIC) || defined(LOCKDEBUG)
	u_long ci_spin_locks;		/* # of spin locks held */
	u_long ci_simple_locks;		/* # of simple locks held */
#endif
#ifdef DIAGNOSTIC
	int	ci_mutex_level;
#endif
	struct device *ci_dev;		/* Device corresponding to this CPU */
	u_int32_t ci_arm_cpuid;		/* aggregate CPU id */
	u_int32_t ci_arm_cputype;	/* CPU type */
	u_int32_t ci_arm_cpurev;	/* CPU revision */
	u_int32_t ci_ctrl;		/* The CPU control register */
	u_int32_t ci_randseed;

	uint32_t ci_cpl;
	uint32_t ci_ipending;
};

#if 0
/* XXX stuff to move to cpuvar.h later */
struct cpu_info {
	u_int32_t ci_arm_cpuid;		/* aggregate CPU id */
	u_int32_t ci_arm_cputype;	/* CPU type */
	u_int32_t ci_arm_cpurev;	/* CPU revision */
	u_int32_t ci_ctrl;		/* The CPU control register */
	uint32_t ci_cpl;


	u_int64_t ci_scratch;
	u_int64_t ci_cur_fsbase;
	struct pcb *ci_curpcb;
	u_int32_t	ci_feature_eflags;
	u_int64_t	ci_tsc_freq;
	void (*ci_info)(struct cpu_info *);
	//struct x86_cache_info ci_cinfo[CAI_COUNT];
	//struct	x86_64_tss *ci_tss;
	
	struct device ci_dev;		/* our device */
	//struct device *ci_dev;		/* Device corresponding to this CPU */
	struct cpu_info *ci_self;	/* pointer to this structure */
	struct schedstate_percpu ci_schedstate; /* scheduler state */
	struct cpu_info *ci_next;	/* next cpu */

	/* 
	 * Public members. 
	 */
	struct proc *ci_curproc; 	/* current owner of the processor */
	struct simplelock ci_slock;	/* lock on this data structure */
	u_int ci_cpuid; 		/* our CPU ID */
	u_int ci_apicid;		/* our APIC ID */
	u_long ci_spin_locks;		/* # of spin locks held */
	u_long ci_simple_locks;		/* # of simple locks held */
	u_int32_t ci_randseed;

	/*
	 * Private members.
	 */
	struct proc *ci_fpcurproc;	/* current owner of the FPU */
	struct proc *ci_fpsaveproc;
	int ci_fpsaving;		/* save in progress */

	//struct pcb *ci_curpcb;		/* VA of current HW PCB */
	struct pcb *ci_idle_pcb;	/* VA of current PCB */
	int ci_idle_tss_sel;		/* TSS selector of idle PCB */
	struct pmap *ci_curpmap;

	//struct intrsource *ci_isources[MAX_INTR_SOURCES];
	u_int32_t	ci_ipending;
	int		ci_ilevel;
	int		ci_idepth;
	//u_int32_t	ci_imask[NIPL];
	//u_int32_t	ci_iunmask[NIPL];
	int		ci_mutex_level;

	paddr_t		ci_idle_pcb_paddr; /* PA of idle PCB */
	volatile u_long	ci_flags;	/* flags; see below */
	u_int32_t	ci_ipis; 	/* interprocessor interrupts pending */

	u_int32_t	ci_level;
	u_int32_t	ci_vendor[4];
	u_int32_t	ci_signature;		/* X86 cpuid type */
	u_int32_t	ci_family;		/* extended cpuid family */
	u_int32_t	ci_model;		/* extended cpuid model */
	u_int32_t	ci_feature_flags;	/* X86 CPUID feature bits */
	u_int32_t	cpu_class;		/* CPU class */
	u_int32_t	ci_cflushsz;		/* clflush cache-line size */

	struct cpu_functions *ci_func;	/* start/stop functions */
	void (*cpu_setup)(struct cpu_info *);	/* proc-dependant init */

	int		ci_want_resched;

	union descriptor *ci_gdt;
	union descriptor *ci_ldt;	/* per-cpu default LDT */
	int		ci_ldt_len;	/* in bytes */

	volatile int ci_ddb_paused;	/* paused due to other proc in ddb */
#define CI_DDB_RUNNING		0
#define CI_DDB_SHOULDSTOP	1
#define CI_DDB_STOPPED		2
#define CI_DDB_ENTERDDB		3
#define CI_DDB_INDDB		4

	volatile int ci_setperf_state;
#define CI_SETPERF_READY	0
#define CI_SETPERF_SHOULDSTOP	1
#define CI_SETPERF_INTRANSIT	2
#define CI_SETPERF_DONE		3

	//struct ksensordev	ci_sensordev;
	//struct ksensor		ci_sensor;
};
#endif

extern struct cpu_info cpu_info_store;
#define	curcpu()	(&cpu_info_store)
#define cpu_number()	0
#define CPU_IS_PRIMARY(ci)	1
#define CPU_INFO_ITERATOR	int
#define CPU_INFO_FOREACH(cii, ci) \
	for (cii = 0, ci = curcpu(); ci != NULL; ci = NULL)
#define CPU_INFO_UNIT(ci)	0
#define MAXCPUS	1
#define cpu_unidle(ci)

/*
 * Scheduling glue
 */

extern int astpending;
#define setsoftast() (astpending = 1)

/*
 * Notify the current process (p) that it has a signal pending,
 * process as soon as possible.
 */

#define signotify(p)            setsoftast()

/*
 * Preempt the current process if in interrupt from user mode,
 * or after the current trap/syscall if in system mode.
 */
extern int want_resched;	/* resched() was called */
#define	need_resched(ci)	(want_resched = 1, setsoftast())
#define clear_resched(ci) 	want_resched = 0

/*
 * Give a profiling tick to the current process when the user profiling
 * buffer pages are invalid.  On the i386, request an ast to send us
 * through trap(), marking the proc as needing a profiling tick.
 */
#define	need_proftick(p)	setsoftast()

/*
 * cpu device glue (belongs in cpuvar.h)
 */

struct device;
void	cpu_attach	(struct device *);
int	cpu_alloc_idlepcb	(struct cpu_info *);

/*
 * Random cruft
 */

/* cpuswitch.S */
struct pcb;
void	savectx		(struct pcb *pcb);

/* machdep.h */
void bootsync		(int);

/* fault.c */
int badaddr_read	(void *, size_t, void *);

/* syscall.c */
//void swi_handler	(trapframe_t *);

/* machine_machdep.c */
void board_startup(void);


#endif /* _KERNEL */

#endif /* !_ARM_CPU_H_ */

/* End of cpu.h */
