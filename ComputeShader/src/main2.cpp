#include "gpgpu.h"
#include "CPU.h"
#include <iostream>
#include <windows.h>

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

void main()
{
	gpu::initGL();

	//std::string DIR = "G:\\�v���o\\Dropbox\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	std::string DIR = "C:\\Users\\��P\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	std::string FILE1 = "p1.glsl";
	std::string FILE2 = "p2.glsl";

	gpu::func<INPUT_DATA, gpu::Empty> func1(DIR+FILE1, num_loop, num_sled);
	gpu::func<gpu::Empty, OUTPUT_DATA> func2(DIR+FILE2, num_loop, num_sled);

	if (func1.checkError() == 1)
	{
		std::cout << "�G���[\n" << func1.getError() << std::endl;
		std::getchar();
		return;
	}

	if (func2.checkError() == 1)
	{
		std::cout << "�G���[\n" << func2.getError() << std::endl;
		std::getchar();
		return;
	}

	LARGE_INTEGER freq;
	if (!QueryPerformanceFrequency(&freq)) return;
	LARGE_INTEGER start, end;

	///
	GLuint aaa;
	glGenBuffers(1, &aaa); // ��̓���SSBO�I�u�W�F�N�g����(�������͐�������I�u�W�F�N�g�̌�)
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, aaa); // ������������SSBO�I�u�W�F�N�g�̃o�C���h
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(OUTPUT_DATA), NULL, GL_STATIC_DRAW); // ����SSBO�I�u�W�F�N�g�̃o�b�t�@�m��
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, aaa); // GPU��RAM�ƕR�����R�Â�(Bind Point�͏d��������邽�߂�SSBO��ID���g�p)
	///

	/////////////////////////////////////////////////////////////////////////////////////////////////

	std::cout << std::endl << "--GPU--" << std::endl;

	// ���o�͍\���̃C���X�^���X��&������
	INPUT_DATA input;
	OUTPUT_DATA output;
	for (int i = 0; i < num_loop; i++) {
		input.a[i] = i;
		output.b[i] = 0.0f;
	}

	// �v�Z
	std::cout << "�v�Z��" << std::endl;
	QueryPerformanceCounter(&start);
	func1.Compute(&input, nullptr);
	func2.Compute(nullptr, &output);
	QueryPerformanceCounter(&end);

	// ���ʏo��
	for (int i = 0; i < num_loop; i++) {
		if (i >= 10) break;
		std::cout << output.b[i] << std::endl;
	}

	// ���v����
	std::cout << "���v����" << std::fixed << (double)(end.QuadPart - start.QuadPart) / freq.QuadPart << "sec.\n";
	std::cout << num_loop << "/48000=" << (double)(num_loop) / 48000.0 << std::endl;
	std::cout <<
		"�c��������:" <<
		((double)(num_loop) / 48000.0) - ((double)(end.QuadPart - start.QuadPart) / freq.QuadPart) <<
		"sec." <<
		std::endl;
	std::cout <<
		"��L�̓��̉�%�������ɗv������:" <<
		(100.0 * 48000.0 * (double)(end.QuadPart - start.QuadPart)) / ((double)(num_loop)* freq.QuadPart) <<
		"%" <<
		std::endl;

	/////////////////////////////////////////////////////////////////////////////////////////////////

	std::cout << std::endl << "--CPU--" << std::endl;

	// ���o�͍\���̃C���X�^���X��&������
	for (int i = 0; i < num_loop; i++) {
		input.a[i] = i;
		output.b[i] = 0.0f;
	}

	// �v�Z
	std::cout << "�v�Z��" << std::endl;
	QueryPerformanceCounter(&start);
	cpu::run<INPUT_DATA, OUTPUT_DATA>(num_loop, &input, &output);
	QueryPerformanceCounter(&end);

	// ���ʏo��
	for (int i = 0; i < num_loop; i++) {
		if (i >= 10) break;
		std::cout << output.b[i] << std::endl;
	}

	// ���v����
	std::cout << "���v����" << std::fixed << (double)(end.QuadPart - start.QuadPart) / freq.QuadPart << "sec.\n";
	std::cout << num_loop << "/48000=" << (double)(num_loop) / 48000.0 << std::endl;
	std::cout <<
		"�c��������:" <<
		((double)(num_loop) / 48000.0) - ((double)(end.QuadPart - start.QuadPart) / freq.QuadPart) <<
		"sec." <<
		std::endl;
	std::cout <<
		"��L�̓��̉�%�������ɗv������:" <<
		(100.0 * 48000.0 * (double)(end.QuadPart - start.QuadPart)) / ((double)(num_loop)* freq.QuadPart) <<
		"%" <<
		std::endl;

	/////////////////////////////////////////////////////////////////////////////////////////////////

	// ���͂�����܂őҋ@
	//std::getchar();

	// �����̏I��
	gpu::exitGL();
	return;
}