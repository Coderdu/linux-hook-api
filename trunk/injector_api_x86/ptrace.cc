/*
 * ptrace.cc
 *
 *  Created on: Aug 13, 2010
 *      Author: fify
 */

/* Attention: This API is designed only for x86 linux! */
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
static const int long_size = sizeof(long);

void ptrace_attach(pid_t pid)
{
	if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0)
	{
		perror("ptrace_attach");
		exit(-1);
	}

	waitpid(pid, NULL, WUNTRACED);

	ptrace_readreg(pid, oldregs);
}

// Never stop
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
	ptrace_writereg(pid, oldregs);

	if (ptrace(PTRACE_DETACH, pid, NULL, NULL) < 0)
	{
		perror("ptrace_detach");
		exit(-1);
	}
}

void ptrace_write(pid_t pid, unsigned long addr, void *vptr, int len)
{
	char *laddr;
	int i, j;

	union u
	{
		long word;
		char chars[long_size];
	}data;

	i = 0;
	j = len / long_size;
	laddr = (char*)vptr;

	while (i < j)
	{
		memcpy(data.chars, laddr, long_size);
		ptrace(PTRACE_POKETEXT, pid, addr + i * 4, data.word);

		++i;

		laddr += long_size;

		if (errno != 0)
			printf("ptrace_write failed\t %ld\n", addr + i * 4);
	}

	j = len % long_size;

	if(j)
	{
		memcpy(data.chars, laddr, j);
		ptrace(PTRACE_POKETEXT, pid, addr + i * 4, data.word);
	}
}

// Read "len" bytes of data and store them to "vptr".
void ptrace_read(pid_t pid, unsigned long addr, void *vptr, int len)
{
	char *laddr;
	int i, j;

	union u
	{
		long word;
		char chars[long_size];
	}data;

	i = 0;
	j = len / long_size;
	laddr = (char *)vptr;

	while(i < j)
	{
		data.word = ptrace(PTRACE_PEEKTEXT, pid, addr + i * 4, NULL);
		memcpy(laddr, data.chars, long_size);
		++i;
		laddr += long_size;
	}

	j = len % long_size;

	if(j)
	{
		data.word = ptrace(PTRACE_PEEKTEXT, pid, addr + i * 4, NULL);
		memcpy(laddr, data.chars, j);
	}
}

// Read a string from the given address.
void ptrace_readstr(pid_t pid, unsigned long addr, void *vptr, int len)
{
	char *str = (char *)vptr;
	int i, count;
	long word;
	char *pa;

	i = count = 0;
	pa = (char *) &word;

	int out = 0;
	while (i < len && !out)
	{
		word = ptrace(PTRACE_PEEKTEXT, pid, addr + count, NULL);
		count += long_size;

		for(int j = 0;j<long_size && !out;j++)
		{
				if(pa[j] == '\0')
				{
						str[i] = '\0';
						out = 1;
				}
				else
						str[i++] = pa[j];
		}

	}
}

void ptrace_readreg(pid_t pid, user_regs_struct &regs)
{
	if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) < 0)
		printf("*** ptrace_readreg error ***\n");

}

void ptrace_writereg(pid_t pid, user_regs_struct &regs)
{
	if (ptrace(PTRACE_SETREGS, pid, NULL, &regs) < 0)
		printf("*** ptrace_writereg error ***\n");
}

// Push the given data to the process' stack and return the stack address.
void * ptrace_push(pid_t pid, void *paddr, int size)
{
	unsigned long esp;
	struct user_regs_struct regs;

	ptrace_readreg(pid, regs);
	esp = regs.esp;
	esp -= size;
	esp = esp - esp % 4;
	regs.esp = esp;

	ptrace_writereg(pid, regs);

	ptrace_write(pid, esp, paddr, size);

	return (void *) esp;
}

// Call a given function specified by its address.
void ptrace_call(pid_t pid, unsigned long addr)
{
	void *pc;
	struct user_regs_struct regs;
	int stat;
	void *pra;

	pc = (void *) 0x41414140;
	pra = ptrace_push(pid, &pc, sizeof(pc));

	ptrace_readreg(pid, regs);
	regs.eip = addr;
	ptrace_writereg(pid, regs);

	ptrace_cont(pid);

	while (!WIFSIGNALED(stat))
		waitpid(pid, &stat, WNOHANG);
}

void ptrace_call_special(pid_t pid, unsigned long addr)
{
	void *pc;
	struct user_regs_struct regs;
	int stat;
	void *pra;

	// 在这里，我们将无效页面地址0x41414140压入目标进程堆栈，这样当程序执行完我们
	// 指定的函数后，程序会产生错误中断，所以我们就又获得了对进程的控制权，可以继
	// 续下面的操作。
	pc = (void *) 0x41414140;
	pra = ptrace_push(pid, &pc, sizeof(pc));

	ptrace_readreg(pid, regs);
	regs.eip = addr;
	ptrace_writereg(pid, regs);

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
