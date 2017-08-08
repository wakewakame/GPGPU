#include "gpgpu.h"
#include <iostream>
#include <windows.h>

struct INPUT_DATA
{
	int a[100];
};

struct OUTPUT_DATA
{
	float b[100];
};

void main()
{
	/*
	��ȗ���
	1.�C���X�^���X����
	2.������
	3.�v���O�����̐���
	4.�v�Z
	*/

	// �G���[�m�F�p�ϐ��̐錾
	bool error;

	// GL�̏�����
	std::cout << "OpenGL�����������Ă��܂�" << std::endl;
	error = gpu::initGL();
	
	// �G���[�m�F
	if (!error)
	{
		std::cout << "OpenGL�̏������Ɏ��s���܂���" << std::endl;
		std::getchar();
		return;
	}

	// GPU���ɒǉ�����֐��̃\�[�X�R�[�h�̃t�@�C���p�X�w��
	std::cout << "GPU���ɒǉ�����֐��̃\�[�X�R�[�h�̃t�@�C���p�X���w�肵�܂�" << std::endl;
		// �t�@�C���I���_�C�A���O�̕\��
		OPENFILENAME ofn;
		char szFile[MAX_PATH] = "";
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.lpstrFilter = TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn) == 0)
		{
			std::cout << "�t�@�C�����I������܂���ł���" << std::endl;
			std::getchar();
			return;
		}

		// �I�����ꂽ�t�@�C���̃p�X�擾
		std::string Filename = ofn.lpstrFile;
		std::cout << Filename << std::endl;

	// GPU���Ɋ֐���ǉ�
	std::cout << "�֐���ǉ����Ă��܂�" << std::endl;
	gpu::func<INPUT_DATA,OUTPUT_DATA> func1(Filename);

	// �G���[�m�F
	if (func1.checkError() == 1)
	{
		std::cout << "�G���[\n" << func1.getError() << std::endl;
		std::getchar();
		return;
	}

	// ���������v���O������ID�\��
	std::cout << "����" << std::endl;
	std::cout << "�v���O����ID:" << func1.getPID() << std::endl;

	// �v�Z
	INPUT_DATA input;
	OUTPUT_DATA output;
	error = func1.Compute(&input, &output);

	// ���͂�����܂őҋ@
	std::getchar();

	// �����̏I��
	gpu::exitGL();
	return;
}