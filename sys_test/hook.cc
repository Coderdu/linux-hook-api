#include <sys/mman.h>
#include <iostream>
#include <fcntl.h>
using namespace std;

int main(int argc, char **argv)
{
		if(argc < 2)
		{
				printf("Please give the pid.\n");
				return 0;
		}

		char mem_name[30];

		sprintf(mem_name, "/proc/%s/mem", *(argv+1));
		printf("Mem file name: %s\n", mem_name);

		int fid = open(mem_name, O_RDWR);
		if(fid < 0)
		{
				perror("Open file failed");
				return -1;
		}

		//unsigned char *p = (unsigned char *)mmap(NULL, 65536, PROT_READ, MAP_SHARED, fid, 0);
		//if(p == MAP_FAILED)
		//{
				//perror("Hook error");
		//}

		//printf("Mapped address: 0x%x\n", (long long)p);
		lseek(fid, 


		return 0;
}

