/*
 * injector.h
 *
 *  Created on: Aug 13, 2010
 *      Author: fify
 */

#ifndef INJECTOR_H_
#define INJECTOR_H_

#include <unistd.h>
#include <elf.h>

namespace Injector
{

void inject_so(pid_t pid, char *so_name);

}

#endif /* INJECTOR_H_ */
