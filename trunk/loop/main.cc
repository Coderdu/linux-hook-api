/*
 * main.cc
 *
 *  Created on: Aug 16, 2010
 *      Author: fify
 */
#include <iostream>
using namespace std;

int main()
{
	for(int i = 0;i<10000;i++)
	{
		cout << "Sleeping: " << i << endl;
		sleep(1);
	}
}
