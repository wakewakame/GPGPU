#pragma once
#include "gpgpu2.h"
#include <iostream>
#include <cmath>
#include <windows.h>

//CPUとGPUで同等の処理を実行する関数群

namespace test {
	const unsigned int num_loop = 1024; // 処理の数
	const unsigned int num_sled = 100; // 処理を何個のスレッドが受け持つか(1<=num_sled<=num_loop)

	struct INPUT_DATA
	{
		int a[num_loop];
	};

	struct OUTPUT_DATA
	{
		float b[num_loop];
	};

	namespace cpu
	{
		INPUT_DATA input;
		OUTPUT_DATA output;

		LARGE_INTEGER freq;
		LARGE_INTEGER start, end;

		// 初期化
		void init() {
			for (int i = 0; i < num_loop; i++) {
				input.a[i] = i;
				output.b[i] = 0.0f;
			}

			if (!QueryPerformanceFrequency(&freq)) return;
		}

		// 演算
		double run() {
			QueryPerformanceCounter(&start);
			for (int i = 0; i < num_loop; i++) {
				for (int j = 0; j < num_loop * 2; j++) {
					output.b[i] = std::sin((float)input.a[i]);
				}
			}
			QueryPerformanceCounter(&end);
			return (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
		}

		// かかった時間表示
		void print() {
			double time = run();
			std::cout << std::fixed << time << "sec.\n";
			/*
			std::cout << num_loop << "/48000=" << (double)(num_loop) / 48000.0 << std::endl;
			std::cout <<
				"残った時間:" <<
				((double)(num_loop) / 48000.0) - time << "sec." << std::endl;
			std::cout <<
				"上記の内の何%を処理に要したか:" <<
				(100.0 * 48000.0 * time / (double)(num_loop)) <<
				"%" <<
				std::endl;
			*/
		}

		// 終了時処理
		void exit() {

		}
	}

	namespace gpu
	{
		std::string DIR = "G:\\思い出\\Dropbox\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
		//std::string DIR = "C:\\Users\\大輝\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
		std::string FILE1 = "p1.glsl";

		gpgpu::var<INPUT_DATA> *input;
		gpgpu::var<OUTPUT_DATA> *output;
		gpgpu::func *func1;

		LARGE_INTEGER freq;
		LARGE_INTEGER start, end;

		// 初期化
		void init() {
			gpgpu::initGL();

			input = new gpgpu::var<INPUT_DATA>;
			output = new gpgpu::var<OUTPUT_DATA>;
			func1 = new gpgpu::func(DIR + FILE1, num_loop, num_sled);

			if (func1->checkError() == 1)
			{
				std::cout << "エラー\n" << func1->getError() << std::endl;
				std::getchar();
				return;
			}

			for (int i = 0; i < num_loop; i++) {
				input->data.a[i] = i;
				output->data.b[i] = 0.0f;
			}

			if (!QueryPerformanceFrequency(&freq)) return;
		}

		// 演算
		double run() {
			QueryPerformanceCounter(&start);
			input->set();
			func1->Compute(*input, *output);
			output->get();
			QueryPerformanceCounter(&end);
			return (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
		}

		// かかった時間表示
		void print() {
			double time = run();
			std::cout <<  std::fixed << time << "sec.\n";
			/*
			std::cout << num_loop << "/48000=" << (double)(num_loop) / 48000.0 << std::endl;
			std::cout <<
				"残った時間:" <<
				((double)(num_loop) / 48000.0) - time << "sec." << std::endl;
			std::cout <<
				"上記の内の何%を処理に要したか:" <<
				(100.0 * 48000.0 * time / (double)(num_loop)) <<
				"%" <<
				std::endl;
			*/
		}

		// 終了時処理
		void exit() {
			delete input;
			delete output;
			delete func1;
			gpgpu::exitGL();
		}
	}
}