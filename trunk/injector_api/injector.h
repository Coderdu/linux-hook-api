/*
 * injector.h
 *
 *  Created on: Aug 13, 2010
 *      Author: fify
 */

#ifndef INJECTOR_H_
#define INJECTOR_H_

#include <unistd.h>

namespace Injector
{

void get_data(pid_t child, long addr, char *str, int len);
void put_data(pid_t child, long addr, char *str, int len);

long long getFreeSpaceAddr(pid_t pid);

}

#endif /* INJECTOR_H_ */
