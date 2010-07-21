#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <stdio.h>

using namespace std;

const int OFFSET = 0x400000;
int main()
{
		printf("My PID: %d\n", getpid());
		char mem_name[25];
		sprintf(mem_name, "/proc/%d/mem", getpid());
		printf("%s\n", mem_name);

		int fid = open(mem_name, O_RDONLY);
		
		unsigned char buf[16000];
		lseek(fid, OFFSET, SEEK_SET);
		read(fid, buf, 16000);

		close(fid);

		for(int i = 0;i<16000;i++)
		{
				if(i && i % 16 == 0)
						printf("\n");
				if(i % 16 == 0)
						printf("0x%08x: ", i + OFFSET);
				printf("%02x ", buf[i]);
		}
		printf("\n");
		sleep(1000);
		return 0;
}

