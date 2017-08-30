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
	DIR = "G:\\思い出\\Dropbox\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	//DIR = "C:\\Users\\大輝\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	FILENAME = "p2.gpu";
	QueryPerformanceFrequency(&freq);

	glewExperimental = GL_TRUE; // よくわかんないけど必要らしい
	if (glfwInit() == GL_FALSE) // GLFW初期化&エラーチェック
	{
		std::cout << "1" << std::endl;
		std::getchar();
		return;
	}
	// OpenGL Version 4.3を指定
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_VISIBLE, 0); // 描画処理はしないので、ウィンドウは生成しないようにする
	GLFWwindow* win = glfwCreateWindow(1, 1, "", NULL, NULL); // ウィンドウ生成
	if (win != nullptr) // GLEW初期化&エラーチェック
	{
		std::cout << "2" << std::endl;
		std::getchar();
		return;
	}
	glfwMakeContextCurrent(win); // 生成したウィンドウをOpenGLの処理対象にする
	if (glewInit() != GLEW_OK) // GLEW初期化&エラーチェック
	{
		std::cout << "3" << std::endl;
		std::getchar();
		return;
	}

	DATA data;

	GLuint SSBO;
	GLvoid* p;
	glGenBuffers(1, &SSBO); // 空の出力SSBOオブジェクト生成(第一引数は生成するオブジェクトの個数)
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO); // 生成した入力SSBOオブジェクトのバインド
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DATA), NULL, GL_STATIC_DRAW); // 入力SSBOオブジェクトのバッファ確保
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO, SSBO); // GPUのRAMと紐ずけ紐づけ(Bind Pointは重複を避けるためにSSBOのIDを使用)
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	for (int j = 0; j < num_loop_y; j++) {
		for (int i = 0; i < num_loop_x; i++) {
			data.arr[i][j] = 0.0f;
		}
	}

	// ソースコードのコンパイル
	bool error;
	std::string error_desc;
	GLuint CSID = glCreateShader(GL_COMPUTE_SHADER); // 空のコンピュートシェーダー生成
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
	)"; // ソースコードをコンピュートシェーダに変換
	char const *charCSCode = CSCode.c_str(); // char型にキャスト
	glShaderSource(CSID, 1, &charCSCode, NULL); // OpenGL側にソースコードを送る
	glCompileShader(CSID); // ソースコードのコンパイル
	GLint Result1 = GL_FALSE; // コンパイル結果代入用変数
	glGetShaderiv(CSID, GL_COMPILE_STATUS, &Result1); // コンパイル結果確認
	if (Result1 == GL_FALSE) // コンパイルに失敗した場合
	{
		const unsigned short MaxErrorLength = 10240; // エラー文の最大文字数
		GLchar charError[MaxErrorLength]; // char型のエラー文を代入する変数
		GLsizei ErrorLength; // エラー文の文字列の長さを代入する変数
		glGetShaderInfoLog(CSID, MaxErrorLength - 1, &ErrorLength, charError); // エラー文取得
		error = 1;
		error_desc = std::string(charError); // エラー文代入
	}
	GLuint CSPID = glCreateProgram(); // 空のプログラム生成
	glAttachShader(CSPID, CSID); // 生成したプログラムに、コンパイルしたシェーダーをアタッチ
	glLinkProgram(CSPID); // 生成したプログラムをリンク
	GLint Result2 = GL_FALSE; // リンク結果代入用変数
	glGetProgramiv(CSPID, GL_LINK_STATUS, &Result2); // リンク結果確認
	if (Result2 == GL_FALSE) // リンクに失敗した場合
	{
		const unsigned short MaxErrorLength = 10240; // エラー文の最大文字数
		GLchar charError[MaxErrorLength]; // char型のエラー文を代入する変数
		GLsizei ErrorLength; // エラー文の文字列の長さを代入する変数
		glGetProgramInfoLog(CSPID, MaxErrorLength - 1, &ErrorLength, charError); // エラー文取得
		error = 1;
		error_desc = std::string(charError); // エラー文代入
	}
	glUseProgram(CSPID);

	QueryPerformanceCounter(&start);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(DATA), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	std::memcpy(p, &data, sizeof(DATA));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glDispatchCompute(num_sled_x, num_sled_y, 1); // 使用するスレッド数(x, y, z)

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
	glfwTerminate(); // GLFWの終了
	return;
}