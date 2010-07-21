#include <iostream>
using namespace std;

int main()
{
		unsigned char *p = (unsigned char *)0x600000;

		for(int i = 0;i<1600;i++)
		{
				if(i && i % 16 == 0)
				{
						printf("\n");
				}
				if(i % 16 == 0)
				{
						printf("%x: ", (long long)p);
				}
				printf("%02x ", *p++);
		}

		printf("\n");
		sleep(1000);
		return 0;
}

