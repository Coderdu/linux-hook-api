/*
 * ptrace.h
 *
 *  Created on: Aug 13, 2010
 *      Author: fify
 */

#ifndef PTRACE_H_
#define PTRACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/user.h>

namespace Injector
{

void ptrace_attach(pid_t pid);
void ptrace_cont(pid_t pid);
void ptrace_detach(pid_t pid);

void ptrace_write(pid_t pid, unsigned long addr, void *vptr, int len);
void ptrace_read(pid_t pid, unsigned long addr, void *vptr, int len);

char * ptrace_readstr(pid_t pid, unsigned long addr);

void ptrace_readreg(pid_t pid, user_regs_struct *regs);
void ptrace_writereg(pid_t pid, struct user_regs_struct *regs);

void * ptrace_push(pid_t pid, void *paddr, int size);

void ptrace_call(pid_t pid, unsigned long addr);
void restart_syscall(void);
void ptrace_call_special(pid_t pid, unsigned long addr);

void ptrace_attach_special(pid_t pid);

} // End of namespace Injector.

#endif /* PTRACE_H_ */
