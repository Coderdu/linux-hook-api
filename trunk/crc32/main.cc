/*
 * main.cc
 *
 *  Created on: Aug 15, 2010
 *      Author: fify
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <iostream>

#include "crc32.h"

int main(void)
{
	char p[4];
	unsigned int long data = 50000;
	memcpy(p, (char*)&data, 4);

	std::cout << crc32(1, (const unsigned char *)p, 4) << std::endl;
	return 0;
}
