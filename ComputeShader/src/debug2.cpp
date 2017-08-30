#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

#include <vector>
#include <fstream>
#include <iterator>
#include <cmath>
#include <windows.h>

std::string DIR;
std::string FILENAME;
LARGE_INTEGER freq;
LARGE_INTEGER start, end;
double time;

#define num_loop_x 1
#define num_loop_y 1
#define num_sled_x 1
#define num_sled_y 1

struct DATA
{
	float arr[num_loop_x][num_loop_y];
};

void main()
{
	DIR = "G:\\�v���o\\Dropbox\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	//DIR = "C:\\Users\\��P\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	FILENAME = "p2.gpu";
	QueryPerformanceFrequency(&freq);

	glewExperimental = GL_TRUE; // �悭�킩��Ȃ����ǕK�v�炵��
	if (glfwInit() == GL_FALSE) // GLFW������&�G���[�`�F�b�N
	{
		std::cout << "1" << std::endl;
		std::getchar();
		return;
	}
	// OpenGL Version 4.3���w��
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_VISIBLE, 0); // �`�揈���͂��Ȃ��̂ŁA�E�B���h�E�͐������Ȃ��悤�ɂ���
	GLFWwindow* win = glfwCreateWindow(1, 1, "", NULL, NULL); // �E�B���h�E����
	if (win != nullptr) // GLEW������&�G���[�`�F�b�N
	{
		std::cout << "2" << std::endl;
		std::getchar();
		return;
	}
	glfwMakeContextCurrent(win); // ���������E�B���h�E��OpenGL�̏����Ώۂɂ���
	if (glewInit() != GLEW_OK) // GLEW������&�G���[�`�F�b�N
	{
		std::cout << "3" << std::endl;
		std::getchar();
		return;
	}

	DATA data;

	GLuint SSBO;
	GLvoid* p;
	glGenBuffers(1, &SSBO); // ��̏o��SSBO�I�u�W�F�N�g����(�������͐�������I�u�W�F�N�g�̌�)
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO); // ������������SSBO�I�u�W�F�N�g�̃o�C���h
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DATA), NULL, GL_STATIC_DRAW); // ����SSBO�I�u�W�F�N�g�̃o�b�t�@�m��
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO, SSBO); // GPU��RAM�ƕR�����R�Â�(Bind Point�͏d��������邽�߂�SSBO��ID���g�p)
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	for (int j = 0; j < num_loop_y; j++) {
		for (int i = 0; i < num_loop_x; i++) {
			data.arr[i][j] = 0.0f;
		}
	}

	// �\�[�X�R�[�h�̃R���p�C��
	bool error;
	std::string error_desc;
	GLuint CSID = glCreateShader(GL_COMPUTE_SHADER); // ��̃R���s���[�g�V�F�[�_�[����
	std::string CSCode = R"(
		#version 430
		layout(std430,binding=1) buffer SSBO1
		{
			float arr[1][1];
		}data;
		layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
		void main
		()
		{
			if(gl_GlobalInvocationID.x >= 1 || gl_GlobalInvocationID.y >= 1 || gl_GlobalInvocationID.z >= 1) return;

			data.arr[gl_GlobalInvocationID.x][gl_GlobalInvocationID.y]=1.0;
		}
	)"; // �\�[�X�R�[�h���R���s���[�g�V�F�[�_�ɕϊ�
	char const *charCSCode = CSCode.c_str(); // char�^�ɃL���X�g
	glShaderSource(CSID, 1, &charCSCode, NULL); // OpenGL���Ƀ\�[�X�R�[�h�𑗂�
	glCompileShader(CSID); // �\�[�X�R�[�h�̃R���p�C��
	GLint Result1 = GL_FALSE; // �R���p�C�����ʑ���p�ϐ�
	glGetShaderiv(CSID, GL_COMPILE_STATUS, &Result1); // �R���p�C�����ʊm�F
	if (Result1 == GL_FALSE) // �R���p�C���Ɏ��s�����ꍇ
	{
		const unsigned short MaxErrorLength = 10240; // �G���[���̍ő啶����
		GLchar charError[MaxErrorLength]; // char�^�̃G���[����������ϐ�
		GLsizei ErrorLength; // �G���[���̕�����̒�����������ϐ�
		glGetShaderInfoLog(CSID, MaxErrorLength - 1, &ErrorLength, charError); // �G���[���擾
		error = 1;
		error_desc = std::string(charError); // �G���[�����
	}
	GLuint CSPID = glCreateProgram(); // ��̃v���O��������
	glAttachShader(CSPID, CSID); // ���������v���O�����ɁA�R���p�C�������V�F�[�_�[���A�^�b�`
	glLinkProgram(CSPID); // ���������v���O�����������N
	GLint Result2 = GL_FALSE; // �����N���ʑ���p�ϐ�
	glGetProgramiv(CSPID, GL_LINK_STATUS, &Result2); // �����N���ʊm�F
	if (Result2 == GL_FALSE) // �����N�Ɏ��s�����ꍇ
	{
		const unsigned short MaxErrorLength = 10240; // �G���[���̍ő啶����
		GLchar charError[MaxErrorLength]; // char�^�̃G���[����������ϐ�
		GLsizei ErrorLength; // �G���[���̕�����̒�����������ϐ�
		glGetProgramInfoLog(CSPID, MaxErrorLength - 1, &ErrorLength, charError); // �G���[���擾
		error = 1;
		error_desc = std::string(charError); // �G���[�����
	}
	glUseProgram(CSPID);

	QueryPerformanceCounter(&start);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(DATA), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	std::memcpy(p, &data, sizeof(DATA));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glDispatchCompute(num_sled_x, num_sled_y, 1); // �g�p����X���b�h��(x, y, z)

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(DATA), GL_MAP_READ_BIT);
	std::memcpy(&data, p, sizeof(DATA));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	QueryPerformanceCounter(&end);
	time = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

	std::cout << "end" << std::endl;

	std::getchar();
	glDeleteShader(CSPID);
	glDeleteBuffers(1, &SSBO);
	glfwTerminate(); // GLFW�̏I��
	return;
}