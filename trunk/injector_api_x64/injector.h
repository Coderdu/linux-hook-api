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

long long getFreeSpaceAddr(pid_t pid);

}

#endif /* INJECTOR_H_ */
