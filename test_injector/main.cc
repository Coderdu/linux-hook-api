/*
 * main.cc
 *
 *  Created on: Aug 15, 2010
 *      Author: fify
 */

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "injector.h"

using namespace std;
using namespace Injector;

char inject_code[] =
{
		// 000000000040053c <main>
		"0xeb0x1c" 						// jmp    40044e <forward>

		// 0000000000400542 <backward>
		"0x5e"							// pop    %rsi
		"0x480xc70xc00x040x000x000x00"	// mov    $0x4,%rax
		"0x480xc70xc30x020x000x000x00"	// mov    $0x2,%rbx
		"0x480x890xf1"					// mov    %rsi,%rcx
		"0x480xc70xc20x0c0x000x000x00"	// mov    $0xc,%rdx
		"0xcd0x80"						// int    $0x80
		"0xcc"							// int3

		// 000000000040055e <forward>
		"0xe80xdf0xff0xff0xff"			// callq  400542 <backward>
		"0x48"							// rex64
		"0x65"							// gs
		"0x6c"							// insb   (%dx),%es:(%rdi)
		"0x6c"							// insb   (%dx),%es:(%rdi)
		"0x6f"							// outsl  %ds:(%rsi),(%dx)
		"0x200x570x6f"					// and    %dl,0x6f(%rdi)
		"0x720x6c"						// jb     4005d9 <__libc_csu_init+0x49>
		"0x640x0a0x00"					// or     %fs:(%rax),%al
		"0xb80x000x000x000x00"			// mov    $0x0,%eax
		"0xc9"							// leaveq
		"0xc3"							// retq
};

int main(int argc, char *argv[])
{
	pid_t traced_process;
	struct user_regs_struct oldregs, regs;

	const int len = sizeof(inject_code);
	char backup[len];
	long long addr;

	if(argc != 2)
	{
		printf("Usage: %s <pid to be traced>\n", argv[0]);
		exit(1);
	}

	traced_process = atoi(argv[1]);

	ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
	wait(NULL);
	ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

	addr = getFreeSpaceAddr(traced_process);

	get_data(traced_process, addr, backup, len);
	put_data(traced_process, addr, inject_code, len);

	memcpy(&oldregs, &regs, sizeof(regs));

	regs.eip = addr;

	ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);
	ptrace(PTRACE_CONT, traced_process, NULL, NULL);

	wait(NULL);

	cout << "Putting back the original code." << endl;

	return 0;
}
