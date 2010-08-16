#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <stdio.h>

using namespace std;

//const long long OFFSET = 0x37a4e00000;
const long long OFFSET = 0x400000;
int main(int argc, char **argv)
{
		if(argc < 2)
		{
				printf("Please give pid.\n");
				return 1;
		}
		printf("My PID: %s\n", *(argv + 1));
		char mem_name[25];
		sprintf(mem_name, "/proc/%s/mem", *(argv + 1));
		printf("%s\n", mem_name);

		int fid = open(mem_name, O_RDWR);
		
		if(fid < 0)
		{
				perror("Open file error");
				return fid;
		}

		unsigned char buf[16000];
		lseek64(fid, OFFSET, SEEK_SET);
		//read(fid, buf, 16000);
		//write(fid, "Hello world", 10);
		//lseek(fid, -10, SEEK_CUR);
		read(fid, buf, 1600);
		//buf[10] = '\0??';
		printf("String: %x\n", buf[0]);

		printf("FID: %d\n", fid);
		sleep(1);
		close(fid);

		for(int i = 0;i<1600;i++)
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

