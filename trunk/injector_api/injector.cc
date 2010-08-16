/*
 * injector.cc
 *
 *  Created on: Aug 13, 2010
 *      Author: fify
 */

#include "injector.h"

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Injector
{
const static int long_size = sizeof(long);

void get_data(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int i, j;
	union u
	{
		long val;
		char chars[long_size];
	}data;
	i = 0;
	j = len / long_size;
	laddr = str;
	while(i < j)
	{
		data.val = ptrace(PTRACE_PEEKDATA, child,
			addr + i * 4, NULL);
		memcpy(laddr, data.chars, long_size);
		++i;
		laddr += long_size;
	}
	j = len % long_size;
	if(j != 0)
	{
		data.val = ptrace(PTRACE_PEEKDATA, child,
			addr + i * 4, NULL);
		memcpy(laddr, data.chars, j);
	}
	str[len] = '\0';
}

void put_data(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int i, j;
	union u
	{
		long val;
		char chars[long_size];
	}data;
	i = 0;
	j = len / long_size;
	laddr = str;
	while(i < j)
	{
		memcpy(data.chars, laddr, long_size);
		ptrace(PTRACE_POKEDATA, child,
			addr + i * 4, data.val);
		++i;
		laddr += long_size;
	}
	j = len % long_size;
	if(j != 0)
	{
		memcpy(data.chars, laddr, j);
		ptrace(PTRACE_POKEDATA, child,
			addr + i * 4, data.val);
	}
}

// Get a pointer to the free space in the process space.
long long getFreeSpaceAddr(pid_t pid)
{
	FILE *fp;
	char filename[30];
	char line[85];
	long long addr = 0;
	char str[20];

	sprintf(filename, "/proc/%d/maps", pid);
	fp = fopen(filename, "r");

	if(fp == NULL)
	{
		exit(1);
	}

	while(fgets(line, 85, fp) != NULL)
	{
		sscanf(line, "%llx-%llx %s %s %s", &addr,
				(long long *)str, str, str, str);
		if(strcmp(str, "00:00") == 0)
			break;
	}

	fclose(fp);

	return addr;
}

} // End of namespace Injector.
