#pragma once
#include "gpgpu2.h"
#include <iostream>
#include <cmath>
#include <windows.h>

// CPUでの処理内容
template <typename INPUT_DATA, typename OUTPUT_DATA>
void cpu_prosess(unsigned int &num_loop, INPUT_DATA &input, OUTPUT_DATA &output) {
	for (int i = 0; i < num_loop; i++) {
		for (int j = 0; j < num_loop; j++)
		{
			output.data.arr[i] += sin(
				input.data.arr[j]
			);
			output.data.arr[i] /= num_loop;
		}	
	}
}

// 使用する変数の型と初期化関数と結果表示関数
struct INPUT_DATA
{
	float arr[4096];
};
struct OUTPUT_DATA
{
	float arr[4096];
};
void var_init(gpgpu::var<INPUT_DATA> &input, gpgpu::var<OUTPUT_DATA> &output)
{
	for (int i = 0; i < 2048; i++)
	{
		input.data.arr[i] = (float)i;
		output.data.arr[i] = 0.0f;
	}
}
void print(gpgpu::var<INPUT_DATA> &input, gpgpu::var<OUTPUT_DATA> &output)
{
	for (int i = 0; i < 1; i++)
	{
		std::cout << "in:" << std::fixed << input.data.arr[i] << ", out:" << std::fixed << output.data.arr[i] << "\n";
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
		gpgpu::var<INPUT_DATA> input;
		gpgpu::var<OUTPUT_DATA> output;

		// cpu
		var_init(input, output);
		{
			QueryPerformanceCounter(&start);
			cpu_prosess(num_loop, input, output);
			QueryPerformanceCounter(&end);
			cpu = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
		}
		// print(input, output);
		// std::cout << "cpu:" << std::fixed << cpu << "sec." << std::endl;
		std::cout << 
			"[cpu]\n" <<
			"  out:" << std::fixed << output.data.arr[0] << "\n" <<
			"  time:" << std::fixed << cpu << "sec." <<
			"\n";

		// gpu
		var_init(input, output);
		gpgpu::func func(DIR + FILE, num_loop, num_sled);
		if (func.checkError())
		{
			std::cout << "エラー\n" << func.getError() << std::endl;
			std::getchar();
			return;
		}
		{
			input.set();
			output.set();
			func.Compute(input, output);
		}
		{
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
		//print(input, output);
		//std::cout << func.getCode() << std::endl;
		//std::cout << "gpu:" << std::fixed << gpu << "sec." << std::endl;
		std::cout <<
			"[gpu]\n" <<
			"  out:" << std::fixed << output.data.arr[0] << "\n" <<
			"  time:" << std::fixed << gpu << "sec." <<
			"\n";

		std::cout << std::endl;
	}
};