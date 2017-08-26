#include "gpgpu2.h"
#include <iostream>
#include <cmath>
#include <windows.h>
#include <iostream>
#include <windows.h>

std::string DIR;
std::string FILENAME1;
std::string FILENAME2;
std::string FILENAME3;
double time;

#define num_sample 1024

struct DATA{
	float arr[num_sample];
};

struct BUFFER{
	float arr[num_sample][num_sample];
};

struct PARAM {
	unsigned int skip;
	unsigned int step;
	unsigned int SamplingHz;
	float StartHz;
	float EndHz;
};

void main()
{
	//DIR = "G:\\évÇ¢èo\\Dropbox\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	DIR = "C:\\Users\\ëÂãP\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	FILENAME1 = "fft1.gpu";
	FILENAME2 = "fft2.gpu";
	FILENAME2 = "fft3.gpu";
	gpgpu::initGL();
	std::cout << "test" << std::endl;

	gpgpu::var<DATA> input;
	gpgpu::var<DATA> output;
	gpgpu::var<BUFFER> buffer;
	gpgpu::var<PARAM> param;

	for (int i = 0; i < num_sample; i++) {
		input.data.arr[i] = 0.0f;
		output.data.arr[i] = 0.0f;
		for (int j = 0; j < num_sample; j++) {
			buffer.data.arr[i][j] = 0.0f;
		}
	}
	param.data = { 4, 0, 44100, 10.0, 1000.0 };

	gpgpu::func mul(DIR + FILENAME1);
	gpgpu::func sum(DIR + FILENAME2);
	gpgpu::func cpy(DIR + FILENAME3);

	mul.set_loop(num_sample, num_sample, num_sample, num_sample);
	sum.set_loop(num_sample, num_sample, num_sample, num_sample);
	cpy.set_loop(num_sample, num_sample);

	input.set();
	param.set();
	mul.Compute(input, buffer, param);
	for (int i = 0; i < ceil(log(num_sample) / log(param.data.skip)); i++) {
		param.set();
		sum.Compute(buffer, param);
		param.data.step++;
	}
	cpy.Compute(buffer, output);
	output.get();

	gpgpu::exitGL();
	std::getchar();

	return;
}