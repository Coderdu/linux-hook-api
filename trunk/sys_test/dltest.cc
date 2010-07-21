#include <dlfcn.h>
#include <stdio.h>

void print_addr(void *printf_call)
{
		long long addr = (long long)printf_call;
		unsigned char *p = (unsigned char*)&addr;
		printf("0x");
		for(int i = 7;i >= 0;i--)
		{
				printf("%02x", *(p+i));
		}
		printf("\n");
}

int foo(int i)
{
		printf("Calling foo: %d\n", i);
		return i;
}

int main(void)
{
		void *libc;
		void (*printf_call)(const char *str, ...);
		void (*printf_call2)(const char *str, ...);
		int (*foo_ptr)(int);
		char *error_text;

		if(libc=dlopen("/lib64/libc.so.6", RTLD_LAZY))
		{
				printf_call = (void(*)(const char *str, ...))dlsym(libc, "printf");
				printf_call2 = (void(*)(const char *str, ...))dlsym(libc, "printf");
				(*printf_call)("Hello World\n");
				dlclose(libc);
				printf("Address for the pointer: ");
				print_addr((void *)printf_call);
				printf("Address for the pointer2: ");
				print_addr((void *)printf_call2);
				printf("Address for the function: ");
				print_addr((void *)printf);

				foo_ptr = foo;

				printf("Address for foo pointer: ");
				print_addr((void *)foo_ptr);
				printf("Address for foo(): ");
				print_addr((void *)foo);
				return 0;
		}


		error_text = dlerror();
		printf(error_text);

		return -2;
}
