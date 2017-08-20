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

		/*
		SSBO�����֐�
			�߂�l:���������1
		*/
		bool createSSBO()
		{
			deleteSSBO(); // SSBO������
			glGenBuffers(1, &SSBO); // ��̏o��SSBO�I�u�W�F�N�g����(�������͐�������I�u�W�F�N�g�̌�)
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO); // ������������SSBO�I�u�W�F�N�g�̃o�C���h
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TYPE), NULL, GL_STATIC_DRAW); // ����SSBO�I�u�W�F�N�g�̃o�b�t�@�m��
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

		// �R���X�g���N�^
		var()
		{
			// �ϐ�������
			SSBO = GL_FALSE;
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
			GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
			// ���
			std::memcpy(p, &data, sizeof(TYPE));
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
			GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			// ���
			std::memcpy(&data, p, sizeof(TYPE));
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
		unsigned int loop; // ���[�v��
		unsigned int sled; // ��������X���b�h��
		bool error; // �G���[�����������1����������
		std::string error_desc; // �G���[������p�ϐ�

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
			std::string CSCode = code.get_glsl(); // �\�[�X�R�[�h���R���s���[�g�V�F�[�_�ɕϊ�
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
				loadShader();
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
		func(std::string Filename, unsigned int set_loop, unsigned int set_sled)
		{
			// �ϐ��̒l�̏�����
			code.open(Filename, 1);
			error = 0;
			error_desc = "";
			loop = set_loop;
			sled = set_sled;
			FID = GL_FALSE;
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

			// �V�F�[�_�[�̎w��
			glUseProgram(FID);

			// �v�Z
			glDispatchCompute(std::ceil(sled), 1, 1); // �g�p����X���b�h��(x, y, z)

			return 1;
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