/*
 * main.cc
 *
 *  Created on: Aug 19, 2010
 *      Author: fify
 */

#include <iostream>
#include <stdlib.h>

#include "injector_api_x86/injector.h"

using namespace std;
using namespace Injector;

int main(int argc, char *argv[])
{
	pid_t pid;
	pid = atoi(argv[1]);

	char so_name[] = {"/home/fify/Project/linux-hook-api/Injected_so/Debug/libInjected_so.so"};
	inject_so(pid, so_name);

	return 0;
}
