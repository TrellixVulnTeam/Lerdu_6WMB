/*	$OpenBSD: device.h,v 1.44 2011/07/03 15:47:16 matthew Exp $	*/
/*	$NetBSD: device.h,v 1.15 1996/04/09 20:55:24 cgd Exp $	*/

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
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
 *	@(#)device.h	8.2 (Berkeley) 2/17/94
 */

#ifndef _SYS_DEVICE_H_
#define	_SYS_DEVICE_H_

#include <sys/queue.h>

/*
 * Minimal device structures.��С�������ṹ��
 * Note that all ``system'' device types are listed here.
 */
#define D_CHR		0x00000001	/* character device */
#define D_BLK		0x00000002	/* block device */
#define D_REM		0x00000004	/* removable device */
#define D_PROT		0x00000008	/* protected device */
#define D_TTY		0x00000010	/* tty device */
enum devclass {
	DV_DULL,		/* generic, no special info ͨ�ã�û���������Ϣ;*/
	DV_CPU,			/* CPU (carries resource utilization)CPU��ִ�е���Դ������??�� */
	DV_DISK,		/* disk drive (label, etc) ��������������ǩ�ȣ�*/
	DV_IFNET,		/* network interface ����ӿ�*/
	DV_TAPE,		/* tape device �Ŵ��豸*/
	DV_TTY			/* serial line interface (???) ������·�ӿ�*/
};

/*
 * Actions for ca_activate.
 */
#define	DVACT_DEACTIVATE	1	/* deactivate the device ͣ���豸*/
#define	DVACT_SUSPEND		2	/* suspend the device ��ͣ�豸*/
#define	DVACT_RESUME		3	/* resume the device �ָ��豸*/
#define	DVACT_QUIESCE		4	/* warn the device about suspend ������ͣ������豸*/

struct device {
	enum	devclass dv_class;	/* this device's classification���豸�ķ��� */
	TAILQ_ENTRY(device) dv_list;	/* entry on list of all devices �Ǽ������豸�б�*/
	struct	cfdata *dv_cfdata;	/* config data that found us ���Ƿ��ֵ���������*/
	int	dv_unit;		/* device unit number �豸��Ԫ��*/
	char	dv_xname[16];		/* external name (name + unit)�ⲿ���ƣ���+��λ�� */
	struct	device *dv_parent;	/* pointer to parent device ���豸��ָ��*/
	int	dv_flags;		/* misc. flags; see below �����־;�ο�����*/
	int	dv_ref;			/* ref count ���ü���*/
};

//prex
struct driver {
	const char	*name;		/* name of device driver */
	struct devops	*devops;	/* device operations */
	size_t		devsz;		/* size of private data */
	int		flags;		/* state of driver */
	int (*probe)	(struct driver *);
	int (*init)	(struct driver *);
	int (*unload)	(struct driver *);
};

//prex
struct devops {
	int (*open)	(device_t, int);
	int (*close)	(device_t);
	int (*read)	(device_t, char *, size_t *, int);
	int (*write)	(device_t, char *, size_t *, int);
	int (*ioctl)	(device_t, u_long, void *);
	int (*devctl)	(device_t, u_long, void *);
};

/* dv_flags */
#define	DVF_ACTIVE	0x0001		/* device is activated �豸������*/

TAILQ_HEAD(devicelist, device);
typedef int (*devop_open_t)   (device_t, int);
typedef int (*devop_close_t)  (device_t);
typedef int (*devop_read_t)   (device_t, char *, size_t *, int);
typedef int (*devop_write_t)  (device_t, char *, size_t *, int);
typedef int (*devop_ioctl_t)  (device_t, u_long, void *);
typedef int (*devop_devctl_t) (device_t, u_long, void *);

#define	no_open		((devop_open_t)nullop)
#define	no_close	((devop_close_t)nullop)
#define	no_read		((devop_read_t)enodev)
#define	no_write	((devop_write_t)enodev)
#define	no_ioctl	((devop_ioctl_t)enodev)
#define	no_devctl	((devop_devctl_t)nullop)

/*
 * Configuration data (i.e., data placed in ioconf.c).
 */
struct cfdata {
	struct	cfattach *cf_attach;	/* config attachment ���ø���*/
	struct	cfdriver *cf_driver;	/* config driver ������������*/
	short	cf_unit;		/* unit number ��Ԫ��*/
	short	cf_fstate;		/* finding state (below) Ѱ��״̬���£�*/
	int	*cf_loc;		/* locators (machine dependent) ��λ����ȡ���ڻ�����*/
	int	cf_flags;		/* flags from config ���õı�־*/
	short	*cf_parents;		/* potential parents Ǳ�ڵĸ�*/
	int	cf_locnames;		/* start of names ��ʼ����*/
	short	cf_starunit1;		/* 1st usable unit number by STAR ��һ��Ԫ��STAR����*/
};
extern struct cfdata cfdata[];
#define FSTATE_NOTFOUND	0	/* has not been found һֱû���ҵ�*/
#define	FSTATE_FOUND	1	/* has been found �ѷ���*/
#define	FSTATE_STAR	2	/* duplicable �ɸ��Ƶ�*/
#define FSTATE_DNOTFOUND 3	/* has not been found, and is disabled ��û�б����֣�������*/
#define FSTATE_DSTAR	4	/* duplicable, and is disabled�ɸ��Ƶģ��������� */

typedef int (*cfmatch_t)(struct device *, void *, void *);
typedef void (*cfscan_t)(struct device *, void *);

/*
 * `configuration' attachment and driver (what the machine-independent
 * autoconf uses).  As devices are found, they are applied against all
 * the potential matches.  The one with the best match is taken, and a
 * device structure (plus any other data desired) is allocated.  Pointers
 * to these are placed into an array of pointers.  The array itself must
 * be dynamic since devices can be found long after the machine is up
 * and running.
 *
 * Devices can have multiple configuration attachments if they attach
 * to different attributes (busses, or whatever), to allow specification
 * of multiple match and attach functions.  There is only one configuration
 * driver per driver, so that things like unit numbers and the device
 * structure array will be shared.
 */
struct cfattach {
	size_t	  ca_devsize;		/* size of dev data (for malloc) / dev�����ݴ�С��Ϊmalloc��*/
	cfmatch_t ca_match;		/* returns a match level ����һ����ͬ����*/
	void	(*ca_attach)(struct device *, struct device *, void *);
	int	(*ca_detach)(struct device *, int);
	int	(*ca_activate)(struct device *, int);
};

/* Flags given to config_detach(), and the ca_detach function. */
#define	DETACH_FORCE	0x01		/* force detachment; hardware gone Ӳ��������*/
#define	DETACH_QUIET	0x02		/* don't print a notice ����ӡ��֪ͨ*/

struct cfdriver {
	void	**cd_devs;		/* devices found �豸����*/
	char	*cd_name;		/* device name �豸����*/
	enum	devclass cd_class;	/* device classification �豸����*/
	int	cd_indirect;		/* indirectly configure subdevices ������ø����豸*/
	int	cd_ndevs;		/* size of cd_devs array / cd_devs����Ĵ�С*/
};

/*
 * Configuration printing functions, and their return codes.  The second
 * argument is NULL if the device was configured; otherwise it is the name
 * of the parent device.  The return value is ignored if the device was
 * configured, so most functions can return UNCONF unconditionally.
 */
typedef int (*cfprint_t)(void *, const char *);
#define	QUIET	0		/* print nothing */
#define	UNCONF	1		/* print " not configured\n" */
#define	UNSUPP	2		/* print " not supported\n" */

//prex
#define	DS_INACTIVE	0x00		/* driver is inactive */
#define DS_ALIVE	0x01		/* probe succeded */
#define DS_ACTIVE	0x02		/* intialized */
/*
 * Pseudo-device attach information (function + number of pseudo-devs).
 */
struct pdevinit {
	void	(*pdev_attach)(int);
	int	pdev_count;
};

#ifdef _KERNEL
struct cftable {
	struct cfdata *tab;
	TAILQ_ENTRY(cftable) list;
};
TAILQ_HEAD(cftable_head, cftable);

extern struct devicelist alldevs;	/* list of all devices �����豸�б�*/

extern int autoconf_verbose;
extern __volatile int config_pending;	/* semaphore for mountroot */

void config_init(void);
void *config_search(cfmatch_t, struct device *, void *);
void *config_rootsearch(cfmatch_t, char *, void *);
struct device *config_found_sm(struct device *, void *, cfprint_t,
    cfmatch_t);
struct device *config_rootfound(char *, void *);
void config_scan(cfscan_t, struct device *);
struct device *config_attach(struct device *, void *, void *, cfprint_t);
int config_detach(struct device *, int);
int config_detach_children(struct device *, int);
int config_deactivate(struct device *);
int config_suspend(struct device *, int);
int config_activate_children(struct device *, int);
struct device *config_make_softc(struct device *parent,
    struct cfdata *cf);
void config_defer(struct device *, void (*)(struct device *));
void config_pending_incr(void);
void config_pending_decr(void);

struct device *device_lookup(struct cfdriver *, int unit);
void device_ref(struct device *);
void device_unref(struct device *);

struct nam2blk {
	char	*name;
	int	maj;
};

int	findblkmajor(struct device *dv);
char	*findblkname(int);
void	setroot(struct device *, int, int);
struct	device *getdisk(char *str, int len, int defpart, dev_t *devp);
struct	device *parsedisk(char *str, int len, int defpart, dev_t *devp);
void	device_register(struct device *, void *);

int loadfirmware(const char *name, u_char **bufp, size_t *buflen);
#define FIRMWARE_MAX	5*1024*1024

/* compatibility definitions */
#define config_found(d, a, p)	config_found_sm((d), (a), (p), NULL)

#endif /* _KERNEL */

#endif /* !_SYS_DEVICE_H_ */
