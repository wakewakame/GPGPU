#include "gpgpu2.h"
#include "RUN2.h"
#include <iostream>
#include <windows.h>

void main()
{
	Test test;
	test.compute(2048*1024,2048);
	std::getchar();
	return;
}