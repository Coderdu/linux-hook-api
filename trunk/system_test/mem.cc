#include <iostream>
using namespace std;

int foo(int i)
{
		return i;
}

int foo1(int i)
{
		return i;
}

int print_addr(int (*foo)(int));

int main()
{
		print_addr(foo);
		print_addr(foo1);

		return 0;
}

int print_addr(int (*foo)(int))
{
		int (*p)(int) = foo;
		long long addr = (long long)p;

		unsigned char *ch = (unsigned char *)&addr;
		for(int i = 0;i<8;i++)
		{
				printf("%02x", *(ch + 7 - i));
		}
		printf("\n");
		return 0;
}

