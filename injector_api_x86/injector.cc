/*
 * injector.cc
 *
 *  Created on: Aug 13, 2010
 *      Author: fify
 */

#include "injector.h"

#include "ptrace.h"
#include "elf_parser.h"

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Injector
{

int _do_inject(pid_t pid, Elf32_Addr dlopen_addr, char *so_file)
{
	char buf1[1024], buf2[1024];
	struct user_regs_struct regs, saved_regs, aregs;

	Elf32_Addr addr;

	char *so = realpath(so_file, NULL);

	if(!so)
	{
		perror("Get full path name");
		return -1;
	}

	ptrace_readreg(pid, regs);
	ptrace_read(pid, regs.esp + 1024, buf1, sizeof(buf1));
	ptrace_read(pid, regs.esp, buf2, sizeof(buf2));

	addr = 0x00;
	ptrace_write(pid, regs.esp, (char *)&addr, sizeof(addr));
	ptrace_write(pid, regs.esp + 1024, so, strlen(so) + 1);
	free(so);

	saved_regs = regs;

	addr = regs.esp + 1024;
	ptrace_write(pid, regs.esp + sizeof(Elf32_Addr), &addr, sizeof(addr));
	addr = RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE;
	ptrace_write(pid, regs.esp + 2 * sizeof(Elf32_Addr), &addr, sizeof(addr));

	regs.eip = dlopen_addr + 2;

	ptrace_writereg(pid, regs);

	ptrace_cont(pid);

	//waitpid(pid, &status, 0);

	ptrace_readreg(pid, aregs);
	ptrace_writereg(pid, saved_regs);

	ptrace_write(pid, saved_regs.esp + 1024, buf1, sizeof(buf1));
	ptrace_write(pid, saved_regs.esp, buf2, sizeof(buf2));

	return 0;
}

void inject_so(pid_t pid, char *so_name)
{
	struct link_map map;
	Elf32_Addr sym_addr;

	ptrace_attach(pid);

	get_linkmap(pid, map);

	sym_addr = find_symbol(pid, &map, (char *)"__libc_dlopen_mode");
	//printf("Symbol Addr: 0x%08x\n", sym_addr);

	_do_inject(pid, sym_addr, so_name);

	ptrace_detach(pid);
}

} // End of namespace Injector.
