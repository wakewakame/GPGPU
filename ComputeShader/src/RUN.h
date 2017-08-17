#pragma once
#include "gpgpu2.h"
#include <iostream>
#include <cmath>
#include <windows.h>

//CPU��GPU�œ����̏��������s����֐��Q

namespace test {
	const unsigned int num_loop = 1024; // �����̐�
	const unsigned int num_sled = 100; // ���������̃X���b�h���󂯎���(1<=num_sled<=num_loop)

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

		// ������
		void init() {
			for (int i = 0; i < num_loop; i++) {
				input.a[i] = i;
				output.b[i] = 0.0f;
			}

			if (!QueryPerformanceFrequency(&freq)) return;
		}

		// ���Z
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

		// �����������ԕ\��
		void print() {
			double time = run();
			std::cout << std::fixed << time << "sec.\n";
			/*
			std::cout << num_loop << "/48000=" << (double)(num_loop) / 48000.0 << std::endl;
			std::cout <<
				"�c��������:" <<
				((double)(num_loop) / 48000.0) - time << "sec." << std::endl;
			std::cout <<
				"��L�̓��̉�%�������ɗv������:" <<
				(100.0 * 48000.0 * time / (double)(num_loop)) <<
				"%" <<
				std::endl;
			*/
		}

		// �I��������
		void exit() {

		}
	}

	namespace gpu
	{
		std::string DIR = "G:\\�v���o\\Dropbox\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
		//std::string DIR = "C:\\Users\\��P\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
		std::string FILE1 = "p1.glsl";

		gpgpu::var<INPUT_DATA> *input;
		gpgpu::var<OUTPUT_DATA> *output;
		gpgpu::func *func1;

		LARGE_INTEGER freq;
		LARGE_INTEGER start, end;

		// ������
		void init() {
			gpgpu::initGL();

			input = new gpgpu::var<INPUT_DATA>;
			output = new gpgpu::var<OUTPUT_DATA>;
			func1 = new gpgpu::func(DIR + FILE1, num_loop, num_sled);

			if (func1->checkError() == 1)
			{
				std::cout << "�G���[\n" << func1->getError() << std::endl;
				std::getchar();
				return;
			}

			for (int i = 0; i < num_loop; i++) {
				input->data.a[i] = i;
				output->data.b[i] = 0.0f;
			}

			if (!QueryPerformanceFrequency(&freq)) return;
		}

		// ���Z
		double run() {
			QueryPerformanceCounter(&start);
			input->set();
			func1->Compute(*input, *output);
			output->get();
			QueryPerformanceCounter(&end);
			return (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
		}

		// �����������ԕ\��
		void print() {
			double time = run();
			std::cout <<  std::fixed << time << "sec.\n";
			/*
			std::cout << num_loop << "/48000=" << (double)(num_loop) / 48000.0 << std::endl;
			std::cout <<
				"�c��������:" <<
				((double)(num_loop) / 48000.0) - time << "sec." << std::endl;
			std::cout <<
				"��L�̓��̉�%�������ɗv������:" <<
				(100.0 * 48000.0 * time / (double)(num_loop)) <<
				"%" <<
				std::endl;
			*/
		}

		// �I��������
		void exit() {
			delete input;
			delete output;
			delete func1;
			gpgpu::exitGL();
		}
	}
}