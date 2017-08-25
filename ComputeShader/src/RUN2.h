#pragma once
#include "gpgpu2.h"
#include <iostream>
#include <cmath>
#include <windows.h>

// CPUでの処理内容
template <typename INPUT_DATA, typename OUTPUT_DATA>
void cpu_prosess(unsigned int &num_loop, INPUT_DATA &input, OUTPUT_DATA &output) {
	for (int i = 0; i < num_loop; i++) {
		output.data.arr[0] += sin((float)i)*input.data.arr[i];
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
struct PARAM
{
	unsigned int times;
};
void var_init(gpgpu::var<INPUT_DATA> &input, gpgpu::var<OUTPUT_DATA> &output, gpgpu::var<PARAM> &param)
{
	for (int i = 0; i < 4096; i++)
	{
		input.data.arr[i] = (float)i;
		output.data.arr[i] = 0.0f;
	}
	param.data.times = 0;
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
		gpgpu::var<PARAM> param;

		// cpu
		var_init(input, output, param);
		{
			QueryPerformanceCounter(&start);
			cpu_prosess(num_loop, input, output);
			QueryPerformanceCounter(&end);
			cpu = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
		}
		print(input, output);
		// std::cout << "cpu:" << std::fixed << cpu << "sec." << std::endl;
		//std::cout << 
		//	"[cpu]\n" <<
		//	"  out:" << std::fixed << output.data.arr[0] << "\n" <<
		//	"  time:" << std::fixed << cpu << "sec." <<
		//	"\n";

		// gpu
		var_init(input, output, param);
		gpgpu::func func(DIR + FILE);
		if (func.checkError())
		{
			std::cout << "エラー\n" << func.getError() << std::endl;
			std::getchar();
			return;
		}
		unsigned int skip = 8;
		func.set_def("skip", std::to_string(skip));
		func.set_loop(num_loop, num_sled, num_loop, num_sled);
		{
			QueryPerformanceCounter(&start);
			input.set();
			output.set();
			for(int i = 0; i < ceil(logf(num_loop) / logf(skip)); i++)
			{
				param.set();
				func.Compute(input, output, param);
				if (func.checkError())
				{
					std::cout << func.getCode() << std::endl;
					std::cout << "エラー\n" << func.getError() << std::endl;
					std::getchar();
					return;
				}
				param.data.times += 1;
			}
			output.get();
			QueryPerformanceCounter(&end);
			gpu = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
		}
		print(input, output);
		//std::cout << "gpu:" << std::fixed << gpu << "sec." << std::endl;
		//std::cout <<
		//	"[gpu]\n" <<
		//	"  out:" << std::fixed << output.data.arr[0] << "\n" <<
		//	"  time:" << std::fixed << gpu << "sec." <<
		//	"\n";

		std::cout <<
			"[time]\n" <<
			"  " << "cpu:" << std::fixed << cpu << "sec." << "," << "gpu:" << std::fixed << gpu << "sec." <<
			"\n";

		//std::cout << func.getCode() << std::endl;
		std::cout << std::endl;
	}
};