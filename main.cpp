#include "ComputeShader/gpgpu.h"
#include <iostream>
#include <cmath>
#include <windows.h>

// GPU���ł̃��[�v��
#define num_loop_x 9
#define num_loop_y 9
// GPU���ɉ��X���b�h��p�ӂ��邩(�����ق�������)
#define num_sled_x 9
#define num_sled_y 9

// GPU���Ƌ��L����\����
struct DATA
{
	float arr[num_loop_x][num_loop_y];
};

void main()
{
	// gpgpu������
	gpgpu::initGL();
	if (gpgpu::error != "") // �G���[�m�F
	{
		std::cout << gpgpu::error << std::endl;
		std::getchar();
		return;
	}

	// gpu���ɕϐ���ǉ�
	gpgpu::var<DATA> data;

	// �ϐ�������
	for (int i = 0; i < num_loop_x * num_loop_y; i++) {
		data.data->arr[i % num_loop_x][i / num_loop_x] = 0.0f;
	}

	// gpu���Ɋ֐���ǉ�
	gpgpu::func func(R"(
SSBO data
{
	float arr[loop.x][loop.y];
};

main()
{
	data.arr[index.x][index.y] = float((1 + index.x) * (1 + index.y));
}
)", false);

	// ���񃋁[�v���邩���w��
	func.set_loop(num_loop_x, num_sled_x, num_loop_y, num_sled_y);

	// gpu���ɕϐ��𑗐M
	data.set();

	// �ϐ���n���Čv�Z
	func.Compute(data);
	if (func.checkError()) // �G���[�m�F
	{
		std::cout << func.getCode() << std::endl;
		std::cout << "�G���[\n" << func.getError() << std::endl;
		std::getchar();
		return;
	}

	// gpu���̕ϐ����擾
	data.get();

	// �v�Z���ʕ\��
	for (int j = 0; j < num_loop_y; j++) {
		for (int i = 0; i < num_loop_x; i++) {
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

	// �I������
	gpgpu::exitGL();

	// �L�[���͂�����܂őҋ@
	std::getchar();
	return;
}