#include "gpgpu2.h"
#include "RUN2.h"
#include <iostream>
#include <windows.h>

void main()
{
	Test test;
	unsigned int num_loop = 2048;
	while (true)
	{
		test.compute(num_loop, num_loop);
		if (std::getchar() == 'e') break;
	}
	return;
}