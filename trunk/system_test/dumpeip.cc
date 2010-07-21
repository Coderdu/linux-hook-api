#include <iostream>
#include <stdio.h>
using namespace std;

int main()
{
		void *p;
		long long n;

		asm
		(
				"call dummy\n\t"
		"dummy:\n\t"
				"pop %0\n\t"
				"movq %0, %%rcx\n\t"
				:"=r"(p)
				:
				:"%rcx"
		);

		printf("%llx\n", (long long)p);

		return 0;
}

