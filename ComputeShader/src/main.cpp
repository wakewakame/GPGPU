#include "../lib/gpgpu.h"
#include <iostream>
#include <cmath>
#include <windows.h>

std::string DIR;
std::string FILENAME;

#define num_loop_x 10
#define num_loop_y 10
#define num_sled_x 10
#define num_sled_y 10

struct DATA
{
	float arr[num_loop_x][num_loop_y];
};

void main()
{
	char DIR_[255];
	GetCurrentDirectory(255, DIR_);
	DIR = std::string(DIR_) + "\\src\\";
	FILENAME = "p2.gpu";

	gpgpu::initGL();
	if (gpgpu::error != "") {
		std::cout << gpgpu::error << std::endl;
		std::getchar();
		return;
	}

	gpgpu::var<DATA> data;

	for (int i = 0; i < num_loop_x * num_loop_y; i++) {
		data.data->arr[i % num_loop_x][i / num_loop_x] = 0.0f;
	}

	gpgpu::func func(DIR + FILENAME);
	func.set_loop(num_loop_x, num_sled_x, num_loop_y, num_sled_y);
	{
		data.set();
		func.Compute(data);
		if (func.checkError())
		{
			std::cout << func.getCode() << std::endl;
			std::cout << "ƒGƒ‰[\n" << func.getError() << std::endl;
			std::getchar();
			return;
		}
		data.get();
	}

	
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 10; i++) {
			std::cout << "[";
			if (data.data->arr[i][j] == 0) std::cout << "  ";
			else for (int num_space = 0; num_space < 2 - (int)floor(log10((float)data.data->arr[i][j])); num_space++) {
				std::cout << " ";
			}
			std::cout << data.data->arr[i][j] << "]";
		}
		if (j == 10) std::cout << std::endl;
		else std::cout << "\n";
	}

	std::getchar();
	gpgpu::exitGL();
	return;
}