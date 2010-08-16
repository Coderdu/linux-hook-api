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
