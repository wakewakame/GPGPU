#pragma once
#include "gpgpu2.h"
#include <iostream>
#include <cmath>
#include <windows.h>

// CPUでの処理内容
template <typename INPUT_DATA, typename OUTPUT_DATA>
void cpu_prosess(unsigned int &num_loop, INPUT_DATA &input, OUTPUT_DATA &output) {
	for (int i = 0; i < num_loop; i++) {
		output.data.sum = (int)(100.0*sin((float)input.data.num));
	}
}

//CPUとGPUで同等の処理を実行する関数群

class Test {
LARGE_INTEGER freq;
LARGE_INTEGER start, end;

std::string DIR;
std::string FILE;

public:
	double cpu, gpu;
	Test() {
		QueryPerformanceFrequency(&freq);
		DIR = "G:\\思い出\\Dropbox\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
		//DIR = "C:\\Users\\大輝\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
		FILE = "p1.gpu";
		gpgpu::initGL();
		cpu = 0.0;
		gpu = 0.0;
	}
	~Test() {
		gpgpu::exitGL();
	}
	void compute(unsigned int num_loop, unsigned int num_sled) {
		struct INPUT_DATA
		{
			int num;
		};
		struct OUTPUT_DATA
		{
			int sum;
		};
		gpgpu::var<INPUT_DATA> input;
		gpgpu::var<OUTPUT_DATA> output;
		{ // cpu
			input.data.num = 1;
			output.data.sum = 0;
			QueryPerformanceCounter(&start);
			cpu_prosess(num_loop, input, output);
			QueryPerformanceCounter(&end);
			cpu = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
		}
		std::cout << output.data.sum << std::endl;
		std::cout << "cpu:" << std::fixed << cpu << "sec." << std::endl;
		{ // gpu
			gpgpu::func func(DIR + FILE, num_loop, num_sled);
			if (func.checkError())
			{
				std::cout << "エラー\n" << func.getError() << std::endl;
				std::getchar();
				return;
			}
			input.data.num = 1;
			output.data.sum = 0;
			{
				input.set();
				output.set();
				func.Compute(input, output);
			}
			QueryPerformanceCounter(&start);
			input.set();
			output.set();
			func.Compute(input, output);
			if (func.checkError())
			{
				std::cout << "エラー\n" << func.getError() << std::endl;
				std::getchar();
				return;
			}
			output.get();
			QueryPerformanceCounter(&end);
			gpu = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
		}
		std::cout << output.data.sum << std::endl;
		//std::cout << func.getCode() << std::endl;
		std::cout << "gpu:" << std::fixed << gpu << "sec." << std::endl;
	}
};