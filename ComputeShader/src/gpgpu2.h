#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <cmath>
#include "transform.h"

// GPGPU�ɕK�v�Ȋ֐���N���X���܂Ƃ߂�����
namespace gpgpu
{
	/*
	�������֐�
	GLFW,GLEW������������Ă��Ȃ���΁A���̊֐��ŏ���������
		�߂�l:���������1
	*/
	bool initGL()
	{
		glewExperimental = GL_TRUE; // �悭�킩��Ȃ����ǕK�v�炵��
		if (glfwInit() == GL_FALSE) // GLFW������&�G���[�`�F�b�N
		{
			return 0;
		}
		// OpenGL Version 4.3���w��
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_VISIBLE, 0); // �`�揈���͂��Ȃ��̂ŁA�E�B���h�E�͐������Ȃ��悤�ɂ���
		GLFWwindow* win = glfwCreateWindow(640, 480, "", NULL, NULL); // �E�B���h�E����
		glfwMakeContextCurrent(win); // ���������E�B���h�E��OpenGL�̏����Ώۂɂ���
		if (glewInit() != GLEW_OK) // GLEW������&�G���[�`�F�b�N
		{
			return 0;
		}
		return 1;
	}

	/*
	�I�������֐�
	initGL���g�p�����ꍇ�A�v���O�����̏I�����ɂ�����Ă�
	*/
	void exitGL()
	{
		glfwTerminate(); // GLFW�̏I��
	}

	// ��̍\���̐錾
	struct Empty{};

	/*
	GPU���ɕϐ���ǉ�����N���X
	�C���X�^���X�𐶐����邲�ƂɁAGPU���ɕϐ����ǉ������
	data�̓e���v���[�g�����̌^�Ő��������ϐ�(TYPE���p�����悤���Ƃ��v�������A�����o���̏d���Ȃǂ����Ȃ̂ł�߂�)
	data�̓��e��set,get�֐���CPU,GPU�̕ϐ��𓯊�����
	set�֐���GPU���ɕϐ���]�����Aget�֐���GPU���̕ϐ����擾����
		�e���v���[�g����1:GPU���ɐ�������\���̂̌^
	*/
	template <typename TYPE>
	class var
	{
	private:
		GLuint SSBO; // SSBO�I�u�W�F�N�g��ID
		unsigned int size; // SSBO�I�u�W�F�N�g�̃T�C�Y

		/*
		SSBO�����֐�
			�߂�l:���������1
		*/
		bool createSSBO()
		{
			deleteSSBO(); // SSBO������
			glGenBuffers(1, &SSBO); // ��̏o��SSBO�I�u�W�F�N�g����(�������͐�������I�u�W�F�N�g�̌�)
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO); // ������������SSBO�I�u�W�F�N�g�̃o�C���h
			glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_STATIC_DRAW); // ����SSBO�I�u�W�F�N�g�̃o�b�t�@�m��
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO, SSBO); // GPU��RAM�ƕR�����R�Â�(Bind Point�͏d��������邽�߂�SSBO��ID���g�p)

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			return 1;
		}

		/*
		SSBO�j���֐�
		*/
		void deleteSSBO()
		{
			if (SSBO != GL_FALSE) {
				glDeleteBuffers(1, &SSBO);
				SSBO = GL_FALSE;
			}
		}

	public:
		// �w�肳�ꂽ�^�̍\���̐���
		TYPE data;

		/*
		�R���X�g���N�^
		�N���X�̃e���v���[�g�����Ɏ��^�ɉϒ��\���̂Ȃǂ�����ꍇ�A
		�R���X�g���N�^�̈����ɔC�ӂ̌^�̃T�C�Y���w��\
			����1:�^�̃T�C�Y(�ȗ��\)
		*/
		var(unsigned int set_size = 0)
		{
			// �ϐ�������
			SSBO = GL_FALSE;
			// �T�C�Y�擾
			if (set_size != 0)
			{
				size = set_size;
			}
			else
			{
				size = sizeof(TYPE);
			}
			// SSBO����
			createSSBO();
		}

		// �f�X�g���N�^
		~var()
		{
			deleteSSBO();
		}

		// GPU���ɕϐ��̓��e��]��
		void set()
		{
			// SSBO�o�C���h
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
			// �\���̃f�[�^����p�̃������m��
			GLvoid* p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			// ���
			std::memcpy(p, &data, size);
			// ��������l��GPU�ɓ]��&���������������̊J��
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			// SSBO�A���o�C���h
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
		void set(TYPE *set_data)
		{
			// SSBO�o�C���h
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
			// �\���̃f�[�^����p�̃������m��
			GLvoid* p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			// ���
			std::memcpy(p, &set_data, size);
			// ��������l��GPU�ɓ]��&���������������̊J��
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			// SSBO�A���o�C���h
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		// GPU���̕ϐ����擾
		void get()
		{
			// SSBO_OUT�o�C���h
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
			// �\���̃f�[�^����p�̃������m��&�l�̎擾
			GLvoid* p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT);
			// ���
			std::memcpy(&data, p, size);
			// ���������������̊J��
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			// SSBO�A���o�C���h
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		// SSBO ID�擾
		GLuint ID() const {
			return SSBO;
		}
	};

	/*
	GPU���Ɋ֐���ǉ�����N���X
	�C���X�^���X�𐶐����邲�ƂɁAGPU�Ɋ֐����ǉ������
	Compute�֐���GPU���̊֐������s����
	�G���[��getError�Ŏ擾�ł���
	*/
	class func
	{
	private:

		Code code; // �V�F�[�_�[�̃\�[�X�R�[�h
		GLuint FID; // GPU���̊֐�ID
		std::vector<GLuint> SSBO_back; // �O���GPU���̕ϐ�ID
		std::vector<GLuint> SSBO; // GPU���̕ϐ�ID
		uvec3 loop; // ���[�v��
		uvec3 sled; // ��������X���b�h��
		std::string CSCode; // GLSL�R�[�h
		bool error; // �G���[�����������1����������
		std::string error_desc; // �G���[������p�ϐ�
		bool compile; // �ăR���p�C���t���O

		/*
		�R���s���[�g�V�F�[�_�[�ǂݍ��݊֐�
			����1:�R���s���[�g�V�F�[�_�̃\�[�X�R�[�h�̃t�@�C���p�X
			�߂�l:���������1
		*/
		bool loadShader()
		{
			// �R���s���[�g�V�F�[�_�[�̔j��
			deleteShader();

			// �\�[�X�R�[�h�̃R���p�C��
			GLuint CSID = glCreateShader(GL_COMPUTE_SHADER); // ��̃R���s���[�g�V�F�[�_�[����
			CSCode = code.get_glsl(); // �\�[�X�R�[�h���R���s���[�g�V�F�[�_�ɕϊ�
			char const *charCSCode = CSCode.c_str(); // char�^�ɃL���X�g
			glShaderSource(CSID, 1, &charCSCode, NULL); // OpenGL���Ƀ\�[�X�R�[�h�𑗂�
			glCompileShader(CSID); // �\�[�X�R�[�h�̃R���p�C��

			// �G���[�m�F
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
				return 0; // GL_FALSE��Ԃ�
			}

			// �v���O�����̐���
			GLuint CSPID = glCreateProgram(); // ��̃v���O��������
			glAttachShader(CSPID, CSID); // ���������v���O�����ɁA�R���p�C�������V�F�[�_�[���A�^�b�`
			glLinkProgram(CSPID); // ���������v���O�����������N

			// �G���[�m�F
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
				return 0; // GL_FALSE��Ԃ�
			}

			// ���������v���O������ID����
			FID = CSPID;

			// �R���p�C���t���O�̉��
			compile = 0;

			// �����̏I��
			return 1;
		}

		// �R���s���[�g�V�F�[�_�[�j���֐�
		void deleteShader() {
			// �V�F�[�_�[�����݂��Ă��邩�m�F
			if (FID != GL_FALSE) {
				// �V�F�[�_�[�̊J��
				glDeleteShader(FID);
				FID = GL_FALSE;
			}
		}
		// Compute�֐��̉ϒ������擾�֐�
		void getArg() // �����̐���0�ɂȂ����Ƃ�
		{
			// �O��̈���ID�ƈႤ�ꍇ�A�V�F�[�_�[�̍ăR���p�C��
			if(SSBO_back != SSBO)
			{
				SSBO = SSBO_back;
				code.set_param("430", loop, sled, (std::vector<unsigned int>)SSBO);
				compile = 1;
			}
			SSBO_back.clear();
		}
		// SSBO_back�Ɉ�����1���ċN�����Œǉ�
		template<typename First, typename ...Rest>
		void getArg(First const &first, Rest const &...rest)
		{
			SSBO_back.push_back(first.ID());
			getArg(rest...);
		}

	public:
		func(std::string Filename)
		{
			// �ϐ��̒l�̏�����
			error = 0;
			error_desc = "";
			FID = GL_FALSE;
			if (!code.open(Filename, 1))
			{
				error = 1;
				error_desc = "File is not found"; // �G���[�����
			}
			compile = 1;
		}
		func(std::string Filename, unsigned int set_loop, unsigned int set_sled)
		{
			// �ϐ��̒l�̏�����
			error = 0;
			error_desc = "";
			loop.x = set_loop;
			if (set_sled > set_loop) sled.x = set_loop;
			else sled.x = set_sled;
			FID = GL_FALSE;
			if (!code.open(Filename, 1))
			{
				error = 1;
				error_desc = "File is not found"; // �G���[�����
			}
			compile = 1;
		}

		~func()
		{
			deleteShader();
		}

		/*
		���񏈗����s�֐�
		����:���o�͂̍\���̂̒��Ƀ|�C���^������ƁAGPU���Œl���Q�Ƃł��Ȃ��Ȃ�̂�NG�����ǁA�Œ蒷�̐��z��Ƃ��Ȃ�OK
			�߂�l:���������1
		*/
		template<typename ...ARG>
		bool Compute(ARG const &...arg)
		{
			// �����̎擾
			getArg(arg...);

			// �ăR���p�C���̕K�v������΁A�ăR���p�C��
			if (compile) loadShader();

			// �V�F�[�_�[�̎w��
			glUseProgram(FID);

			// �v�Z
			glDispatchCompute(sled.x, sled.y, sled.z); // �g�p����X���b�h��(x, y, z)

			return 1;
		}

		/*
		���[�v�񐔂Ǝg�p����X���b�h���w��
			����1,3,5:x,y,z�Ń��[�v�������(2���ڈȍ~�͏ȗ���)
			����2,4,6:x,y,z�Ŏg�p����X���b�h��(2���ڈȍ~�͏ȗ���)
		*/
		void set_loop(
			unsigned int set_loop_x,
			unsigned int set_sled_x,
			unsigned int set_loop_y = 1,
			unsigned int set_sled_y = 1,
			unsigned int set_loop_z = 1,
			unsigned int set_sled_z = 1
		)
		{
			// ���
			loop.x = set_loop_x;
			if (set_sled_x > set_loop_x) sled.x = set_loop_x;
			else sled.x = set_sled_x;
			loop.y = set_loop_y;
			if (set_sled_y > set_loop_y) sled.y = set_loop_y;
			else sled.y = set_sled_y;
			loop.z = set_loop_z;
			if (set_sled_z > set_loop_z) sled.z = set_loop_z;
			else sled.z = set_sled_z;
			// �ăR���p�C���t���O
			compile = 1;
		}

		/*
		�R�[�h��define�ǉ��֐�
			����1:�萔��
			����2:�萔�l
		*/
		void set_def(std::string def_name, std::string def_val)
		{
			code.set_define(def_name, def_val);
			compile = 1;
		}

		// �R�[�h�m�F
		std::string getCode()
		{
			return CSCode;
		}

		// �G���[�m�F
		bool checkError()
		{
			return error;
		}

		// �G���[���擾�֐�
		std::string getError()
		{
			return error_desc;
		}

		// ���������R���s���[�g�V�F�[�_�[�̃v���O����ID�擾�֐�
		GLuint getPID()
		{
			return FID;
		}
	};
}