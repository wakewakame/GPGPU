#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iterator>

// GPGPU�ɕK�v�Ȋ֐���N���X���܂Ƃ߂�����
namespace gpu
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

	/*
	GPU���Ɋ֐���ǉ�����N���X
	�C���X�^���X�𐶐����邲�ƂɁAGPU�Ɋ֐����ǉ������
	Compute�֐���GPU���̊֐������s����
	�G���[��getError�Ŏ擾�ł���
		�e���v���[�g����1:GPU���ɒl��n�����߂̓��͍\����(�������void�^���w��)
		�e���v���[�g����2:GPU������l���󂯎�邽�߂̏o�͍\����(�������void�^���w��)
	*/
	template <typename INPUT, typename OUTPUT>
	class func
	{
	private:
		GLuint FID; // GPU���̊֐�ID
		GLuint SSBO_IN; // ����SSBO�I�u�W�F�N�g��ID
		GLuint SSBO_OUT; // �o��SSBO�I�u�W�F�N�g��ID
		bool error; // �G���[�����������1����������
		std::string error_desc; // �G���[������p�ϐ�

		/*
		�e�L�X�g�t�@�C���ǂݍ��݊֐�
			����1:�e�L�X�g�̃t�@�C���̃p�X
			����2:�ǂݍ��񂾃e�L�X�g��������ϐ�
			�߂�l:���������1
		*/
		bool loadText(std::string Filename, std::string *Text)
		{
			*Text = ""; // �e�L�X�g������
			std::ifstream ifsText(Filename); // ifstream�Ńe�L�X�g�ǂݍ���
			if (ifsText.fail()) // �����m�F
			{
				error = 1;
				error_desc = "File Open Failed";
				return 0;
			}
			// �g�����Ƃ��d�g�݂��悭�킩��Ȃ����ǁA����œǂݍ��߂�̂�OK�B
			std::istreambuf_iterator<char> it(ifsText);
			std::istreambuf_iterator<char> last;
			std::string _Text(it, last);
			*Text = _Text; // ���
			return 1;
		}

		/*
		�R���s���[�g�V�F�[�_�[�ɕϊ�����֐�
			����1:�ϊ�����e�L�X�g
			�߂�l:�ϊ����ꂽ�e�L�X�g
		*/
		void Translation(std::string *SorceCode)
		{
			/*
			// �V���O���N�H�[�e�[�V�����A�_�u���N�H�[�e�[�V�������͖���

			// �R�����g�A�E�g�̏���

			// �V�F�[�_�̃o�[�W�����w��
			*SorceCode = "#version 430\n" + *SorceCode;
			*/

			/*
			�R�����g�A�E�g����->�R�����g����\n�ȊO�̉��s�R�[�h����->for���ŕ����񕶃��[�v->"",(),{}�Ȃǂ̈͂��������邲�Ƃ�Tree�`��
			�C���f���g��X�y�[�X�͂��̂܂܂ɂ��Ă���
			Tree�̃m�[�h�ɂ́A�ق���"",(),{}�Ȃǂ̃m�[�h�A����ȊO��,��;�ŋ�؂����e�L�X�g�m�[�h�A���m�[�h��"",(),{},,,;�Ȃǂ̑����̏���ێ�������
			""���͏������Ȃ� \" �ɒ���
			()����,��;�ŋ�؂�
			{}���͒ʏ�ʂ�ɏ���

			INPUT_DATA��OUTPUT_DATA,SLED�̃m�[�h��T���A�c���[��u��
			�c���[�𕶎���ɍČ`��
			*/

			return;
		}

		/*
		�R���s���[�g�V�F�[�_�[�ǂݍ��݊֐�
			����1:�R���s���[�g�V�F�[�_�̃\�[�X�R�[�h�̃t�@�C���p�X
			�߂�l:���������1
		*/
		bool loadShader(std::string Filename)
		{
			// �ϐ��̒l�̏�����
			FID = GL_FALSE;
			SSBO_IN = GL_FALSE;
			SSBO_OUT = GL_FALSE;

			// SSBO�̐���
			if (typeid(INPUT) != typeid(void)) // GPU�ɑ���f�[�^�����邩�m�F
			{
				glGenBuffers(1, &SSBO_IN); // ��̓���SSBO�I�u�W�F�N�g����(�������͐�������I�u�W�F�N�g�̌�)
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_IN); // ������������SSBO�I�u�W�F�N�g�̃o�C���h
				glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(INPUT), NULL, GL_STATIC_DRAW); // ����SSBO�I�u�W�F�N�g�̃o�b�t�@�m��
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_IN, SSBO_IN); // GPU��RAM�ƕR�����R�Â�(Bind Point�͏d��������邽�߂�SSBO��ID���g�p)
			}
			if (typeid(OUTPUT) != typeid(void)) // GPU����󂯎��f�[�^�����邩�m�F
			{
				glGenBuffers(1, &SSBO_OUT); // ��̏o��SSBO�I�u�W�F�N�g����(�������͐�������I�u�W�F�N�g�̌�)
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_OUT); // ������������SSBO�I�u�W�F�N�g�̃o�C���h
				glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(OUTPUT), NULL, GL_STATIC_DRAW); // ����SSBO�I�u�W�F�N�g�̃o�b�t�@�m��
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_OUT, SSBO_OUT); // GPU��RAM�ƕR�����R�Â�(Bind Point�͏d��������邽�߂�SSBO��ID���g�p)
			}
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			/*
			SSBO�Ɋւ���Q�lURL
			http://techblog.sega.jp/entry/2016/10/27/140454
			http://hmgmmsa.hatenablog.com/entry/2017/05/05/041200
			http://www.geeks3d.com/20140704/tutorial-introduction-to-opengl-4-3-shader-storage-buffers-objects-ssbo-demo/
			���̂ւ�

			SSBO�̃o�b�t�@�m�ۂ�
			glBufferData
			SSBO�Ƀf�[�^�����
			glBufferSubData
			glBindBufferBase
			���̂ւ�H
			*/

			// �\�[�X�R�[�h�̃R���p�C��
			GLuint CSID = glCreateShader(GL_COMPUTE_SHADER); // ��̃R���s���[�g�V�F�[�_�[����
			std::string CSCode; // �\�[�X�R�[�h�̕��������p�ϐ�
			if (!loadText(Filename, &CSCode)) // �\�[�X�R�[�h�̓ǂݍ���&�G���[�m�F
			{
				return 0;
			}

			// �R���s���[�g�V�F�[�_�ɕϊ�
			Translation(&CSCode);
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

	public:
		func(std::string Filename)
		{
			// �ϐ��̒l�̏�����
			error = 0;
			error_desc = "";
			loadShader(Filename);
		}

		~func()
		{

		}

		/*
		���񏈗����s�֐�
		����:���o�͂̍\���̂̒��Ƀ|�C���^������ƁAGPU���Œl���Q�Ƃł��Ȃ��Ȃ�̂�NG�����ǁA�Œ蒷�̐��z��Ƃ��Ȃ�OK
			����1:���͍\���̂̃|�C���^(�������nullptr���w��)
			����2:�o�͍\���̂̃|�C���^(�������nullptr���w��)
			�߂�l:���������1
		*/
		bool Compute(INPUT *input, OUTPUT *output)
		{
			// �V�F�[�_�[�̎w��
			glUseProgram(FID);

			// �l�̑��
			if ((typeid(INPUT) != typeid(void)) && (input != nullptr)) // GPU�ɑ���f�[�^�����邩�m�F
			{
				// SSBO_IN�o�C���h
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_IN);
				// �o�C���f�B���O�|�C���g�̎w��
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBO_IN); // Binding Point�͏d��������邽�߂�SSBO��ID���g�p
				// �\���̃f�[�^����p�̃������m��
				GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
				// ���
				std::memcpy(p, input, sizeof(INPUT));
				// ��������l��GPU�ɓ]��&���������������̊J��
				glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			}

			// �v�Z
			glDispatchCompute(512 / 100, 1, 1); // �g�p����X���b�h��

			// �v�Z���ʂ̎擾
			if ((typeid(OUTPUT) != typeid(void)) && (output != nullptr)) // GPU����󂯎��f�[�^�����邩�m�F
			{
				// SSBO_OUT�o�C���h
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_OUT);
				// �o�C���f�B���O�|�C���g�̎w��
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO_OUT); // Binding Point�͏d��������邽�߂�SSBO��ID���g�p
				// �\���̃f�[�^����p�̃������m��&�l�̎擾
				GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
				// ���
				std::memcpy(output, p, sizeof(OUTPUT));
				// ���������������̊J��
				glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			}
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