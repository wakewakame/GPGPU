#include "gpgpu2.h"
#include <iostream>
#include <cmath>
#include <windows.h>
#include <iostream>
#include <windows.h>

std::string DIR;
std::string FILENAME;
LARGE_INTEGER freq;
LARGE_INTEGER start, end;
double time;

#define num_loop_x 9
#define num_loop_y 9
#define num_sled_x 9
#define num_sled_y 9

struct DATA
{
	float arr[num_loop_x][num_loop_y];
};

void main()
{
	DIR = "G:\\思い出\\Dropbox\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	//DIR = "C:\\Users\\大輝\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	FILENAME = "p2.gpu";
	QueryPerformanceFrequency(&freq);
	gpgpu::initGL();

	gpgpu::var<DATA> data;

	for (int i = 0; i < num_loop_x * num_loop_y; i++) {
		data.data.arr[i % num_loop_x][i / num_loop_x] = 0.0f;
	}

	gpgpu::func func(DIR + FILENAME);
	func.set_loop(num_loop_x, num_sled_x, num_loop_y, num_sled_y);
	{
		QueryPerformanceCounter(&start);
		data.set();
		func.Compute(data);
		if (func.checkError())
		{
			std::cout << func.getCode() << std::endl;
			std::cout << "エラー\n" << func.getError() << std::endl;
			std::getchar();
			return;
		}
		data.get();
		QueryPerformanceCounter(&end);
		time = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
	}

	for (int j = 0; j < num_loop_y; j++) {
		for (int i = 0; i < num_loop_x; i++) {
			std::cout << "[";
			if (data.data.arr[i][j] == 0) std::cout << "  ";
			else for (int num_space = 0; num_space < 2 - (int)floor(log10((float)data.data.arr[i][j])); num_space++) {
				std::cout << " ";
			}
			std::cout << data.data.arr[i][j] << "]";
		}
		if (j == num_loop_y) std::cout << std::endl;
		else std::cout << "\n";
	}

	std::getchar();
	gpgpu::exitGL();
	return;
}