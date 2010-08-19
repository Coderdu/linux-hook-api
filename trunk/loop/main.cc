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

int main()
{
	pid_t pid = open("file.txt", O_RDONLY );
	int tmp;
	for(int i = 0;i<3000;i++)
	{
		//cout << "Sleeping: " << i << endl;

		//write(pid, &i, sizeof(i));
		//seek(pid, -1);
		read(pid, &tmp, sizeof(tmp));

		printf("[0x%08x]\n", tmp);
		sleep(1);
	}

	close(pid);
}
