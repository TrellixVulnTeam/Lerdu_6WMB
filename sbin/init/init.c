/*
 * Copyright (c) 2005-2008, Kohsuke Ohtani
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
 * 3. Neither the name of the author nor the names of any co-contibutors
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

/*
 * init.c - traditional POSIX init.
 *
 * Required capabilities:
 *      CAP_NICE, CAP_KILL, CAP_RAWIO
 */


#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static const char cmdbox[] = "/boot/cmdbox";
static const char sh[]     = "sh";
static const char runcom[] = "/etc/rc";
static const char ctty[]   = "/dev/console";

static jmp_buf jbuf;

/*
 * The mother of all processes.
 */
int
main(int argc, char *argv[])
{
	pid_t pid;

	sys_log("init: booting\n");
	setsid();

	setjmp(jbuf);

	close(0);
	close(1);
	close(2);

	pid = vfork();

	if (pid == -1)
		exit(1);

	if (pid == 0) {
		/*
		 * Child
		 */
		signal(SIGHUP, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGALRM, SIG_DFL);
		setsid();
      		open(ctty, O_RDWR);	/* stdin */
		dup(0);			/* stdout */
		dup(0);			/* stderr */

		sys_log("init: running boot script\n");
		execl(cmdbox, sh, runcom);
		sys_panic("init: no shell");
	}

	thread_setpri(thread_self(), 254);
	while (wait((int *)0) != pid)
		;

	/*
	 * Login shell is terminated.
	 */
	sys_log("init: restarting...\n");
	sync();
	kill(-1, SIGTERM);
	sleep(1);
	longjmp(jbuf, 1);

	/* NOTREACHED */
	return 0;
}
