/*
 * Copyright (c) 2007, Kohsuke Ohtani
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



#include <ipc/proc.h>
#include <sys/resource.h>

#include <stddef.h>
#include <unistd.h>
#include <errno.h>

/*
 * setpriority - set priority for process or process group.
 *
 * Note: support only for current process.
 */
int
setpriority(int which, id_t who, int pri)
{
	int error, val;

	switch (which) {
	case PRIO_PROCESS:
	case PRIO_PGRP:
		if (who != 0) {
			errno = EPERM;
			return -1;
		}
		break;
	case PRIO_USER:
		if (who != 1) {
			errno = EPERM;
			return -1;
		}
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	if (pri < MINPRI)
		pri = MINPRI;
	if (pri > MAXPRI)
		pri = MAXPRI;

	val = PRI_DEFAULT + pri;

	error = thread_setpri(thread_self(), val);
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}
