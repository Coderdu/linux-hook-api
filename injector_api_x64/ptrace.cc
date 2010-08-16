/*
 * ptrace.cc
 *
 *  Created on: Aug 13, 2010
 *      Author: fify
 */

#include "ptrace.h"

#include <sys/ptrace.h>
#include <sys/user.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

namespace Injector
{
static struct user_regs_struct oldregs;

void ptrace_attach(pid_t pid)
{
	if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0)
	{
		perror("ptrace_attach");
		exit(-1);
	}

	waitpid(pid, NULL, WUNTRACED);

	ptrace_readreg(pid, &oldregs);
#if 0
	restart_syscall();
#endif
}

void ptrace_cont(pid_t pid)
{
	int stat;

	if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0)
	{
		perror("ptrace_cont");
		exit(-1);
	}

	while (!WIFSTOPPED(stat))
		waitpid(pid, &stat, WNOHANG);
}

void ptrace_detach(pid_t pid)
{
	ptrace_writereg(pid, &oldregs);

	if (ptrace(PTRACE_DETACH, pid, NULL, NULL) < 0)
	{
		perror("ptrace_detach");
		exit(-1);
	}
}

void ptrace_write(pid_t pid, unsigned long addr, void *vptr, int len)
{
	int count;
	long word;

	count = 0;

	while (count < len)
	{
		memcpy(&word, (char *)vptr + count, sizeof(word));
		word = ptrace(PTRACE_POKETEXT, pid, addr + count, word);
		count += 4;

		if (errno != 0)
			printf("ptrace_write failed\t %ld\n", addr + count);
	}
}

void ptrace_read(pid_t pid, unsigned long addr, void *vptr, int len)
{
	int i, count;
	long word;
	unsigned long *ptr = (unsigned long *) vptr;

	i = count = 0;

	while (count < len)
	{
		word = ptrace(PTRACE_PEEKTEXT, pid, addr + count, NULL);
		count += 4;
		ptr[i++] = word;
	}
}

char * ptrace_readstr(pid_t pid, unsigned long addr)
{
	char *str = (char *) malloc(64);
	int i, count;
	long word;
	char *pa;

	i = count = 0;
	pa = (char *) &word;

	while (i <= 60)
	{
		word = ptrace(PTRACE_PEEKTEXT, pid, addr + count, NULL);
		count += 4;

		if (pa[0] == '\0')
		{
			str[i] = '\0';
			break;
		}
		else
			str[i++] = pa[0];

		if (pa[1] == '\0')
		{
			str[i] = '\0';
			break;
		}
		else
			str[i++] = pa[1];

		if (pa[2] == '\0')
		{
			str[i] = '\0';
			break;
		}
		else
			str[i++] = pa[2];

		if (pa[3] == '\0')
		{
			str[i] = '\0';
			break;
		}
		else
			str[i++] = pa[3];
	}

	return str;
}

void ptrace_readreg(pid_t pid, user_regs_struct *regs)
{
	if (ptrace(PTRACE_GETREGS, pid, NULL, regs))
		printf("*** ptrace_readreg error ***\n");

}

void ptrace_writereg(pid_t pid, user_regs_struct *regs)
{
	if (ptrace(PTRACE_SETREGS, pid, NULL, regs))
		printf("*** ptrace_writereg error ***\n");
}

void * ptrace_push(pid_t pid, void *paddr, int size)
{
	unsigned long esp;
	struct user_regs_struct regs;

	ptrace_readreg(pid, &regs);
	esp = regs.esp;
	esp -= size;
	esp = esp - esp % 4;
	regs.esp = esp;

	ptrace_writereg(pid, &regs);

	ptrace_write(pid, esp, paddr, size);

	return (void *) esp;
}

void ptrace_call(pid_t pid, unsigned long addr)
{
	void *pc;
	struct user_regs_struct regs;
	int stat;
	void *pra;

	pc = (void *) 0x41414140;
	pra = ptrace_push(pid, &pc, sizeof(pc));

	ptrace_readreg(pid, &regs);
	regs.eip = addr;
	ptrace_writereg(pid, &regs);

	ptrace_cont(pid);

	while (!WIFSIGNALED(stat))
		waitpid(pid, &stat, WNOHANG);
}

void restart_syscall(void)
{
	if (oldregs.eax != oldregs.orig_eax && oldregs.orig_eax != 0xffffff00)
	{
		oldregs.eip -= 2;
		oldregs.eax = oldregs.orig_eax;
	}
}

void ptrace_call_special(pid_t pid, unsigned long addr)
{
	void *pc;
	struct user_regs_struct regs;
	int stat;
	void *pra;

	pc = (void *) 0x41414140;
	pra = ptrace_push(pid, &pc, sizeof(pc));

	ptrace_readreg(pid, &regs);
	regs.eip = addr;
	ptrace_writereg(pid, &regs);

	ptrace_cont(pid);
	ptrace_detach(pid);
	stat = 0;
	while (!WIFSIGNALED(stat))
		waitpid(pid, &stat, WNOHANG);
}

void ptrace_attach_special(pid_t pid)
{
	void *pc;
	void *pra;

	if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0)
	{
		perror("ptrace_attach");
		exit(-1);
	}

	waitpid(pid, NULL, WUNTRACED);
	pc = (void *) 0x41414140;
	pra = ptrace_push(pid, &pc, sizeof(pc));
}

}
