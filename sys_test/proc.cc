#include <stdio.h>
#include <unistd.h>

void print_addr(long long lli);

int foo(int i)
{
		return i;
}

int main()
{
		printf("main(): ");
		print_addr((long long)main);
		printf("foo(): ");
		print_addr((long long)foo);
		printf("print_addr(): ");
		print_addr((long long)print_addr);
		printf("sleep(): ");
		print_addr((long long)sleep);

		for(int i = 0;i<1000;i++)
		{
				sleep(10);
				printf("sleep: %d\n", 10 * i);
		}
		return 0;
}

void print_addr(long long p)
{
		long long lli = (long long)p;

		char *ch = (char *)&lli;

		for(int i = 7;i>=0;i--)
		{
				printf("%02x", *(ch + i));
		}
		printf("\n");
}

