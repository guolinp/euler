#include <iostream>
using namespace std;

#include "c_fun.h"

void cpp_function(void)
{
	cout<<"this is a cpp function"<<endl;
}

int main()
{
	c_function();
	cpp_function();
	return 0;
}
