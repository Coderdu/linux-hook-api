/*
 * main.cc
 *
 *  Created on: Aug 16, 2010
 *      Author: fify
 */
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

const int MAX_LOOP = 6000;
int main()
{
	pid_t pid = open("file.txt", O_RDONLY | O_CREAT);
	int tmp;
	for(int i = 0;i<MAX_LOOP;i++)
	{
		//cout << "Sleeping: " << i << endl;

		//write(pid, &i, sizeof(i));
		//seek(pid, -1);
		read(pid, &tmp, sizeof(tmp));

		printf("[0x%08x][0x%08x]\n", tmp, MAX_LOOP - tmp);
		sleep(1);
	}

	close(pid);
}
