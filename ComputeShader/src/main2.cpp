#include "gpgpu.h"
#include "CPU.h"
#include <iostream>
#include <windows.h>

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

void main()
{
	gpu::initGL();

	//std::string DIR = "G:\\思い出\\Dropbox\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	std::string DIR = "C:\\Users\\大輝\\Dropbox\\C++\\ComputeShader\\ComputeShader\\src\\";
	std::string FILE1 = "p1.glsl";
	std::string FILE2 = "p2.glsl";

	gpu::func<INPUT_DATA, gpu::Empty> func1(DIR+FILE1, num_loop, num_sled);
	gpu::func<gpu::Empty, OUTPUT_DATA> func2(DIR+FILE2, num_loop, num_sled);

	if (func1.checkError() == 1)
	{
		std::cout << "エラー\n" << func1.getError() << std::endl;
		std::getchar();
		return;
	}

	if (func2.checkError() == 1)
	{
		std::cout << "エラー\n" << func2.getError() << std::endl;
		std::getchar();
		return;
	}

	LARGE_INTEGER freq;
	if (!QueryPerformanceFrequency(&freq)) return;
	LARGE_INTEGER start, end;

	///
	GLuint aaa;
	glGenBuffers(1, &aaa); // 空の入力SSBOオブジェクト生成(第一引数は生成するオブジェクトの個数)
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, aaa); // 生成した入力SSBOオブジェクトのバインド
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(OUTPUT_DATA), NULL, GL_STATIC_DRAW); // 入力SSBOオブジェクトのバッファ確保
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, aaa); // GPUのRAMと紐ずけ紐づけ(Bind Pointは重複を避けるためにSSBOのIDを使用)
	///

	/////////////////////////////////////////////////////////////////////////////////////////////////

	std::cout << std::endl << "--GPU--" << std::endl;

	// 入出力構造体インスタンス化&初期化
	INPUT_DATA input;
	OUTPUT_DATA output;
	for (int i = 0; i < num_loop; i++) {
		input.a[i] = i;
		output.b[i] = 0.0f;
	}

	// 計算
	std::cout << "計算中" << std::endl;
	QueryPerformanceCounter(&start);
	func1.Compute(&input, nullptr);
	func2.Compute(nullptr, &output);
	QueryPerformanceCounter(&end);

	// 結果出力
	for (int i = 0; i < num_loop; i++) {
		if (i >= 10) break;
		std::cout << output.b[i] << std::endl;
	}

	// 所要時間
	std::cout << "所要時間" << std::fixed << (double)(end.QuadPart - start.QuadPart) / freq.QuadPart << "sec.\n";
	std::cout << num_loop << "/48000=" << (double)(num_loop) / 48000.0 << std::endl;
	std::cout <<
		"残った時間:" <<
		((double)(num_loop) / 48000.0) - ((double)(end.QuadPart - start.QuadPart) / freq.QuadPart) <<
		"sec." <<
		std::endl;
	std::cout <<
		"上記の内の何%を処理に要したか:" <<
		(100.0 * 48000.0 * (double)(end.QuadPart - start.QuadPart)) / ((double)(num_loop)* freq.QuadPart) <<
		"%" <<
		std::endl;

	/////////////////////////////////////////////////////////////////////////////////////////////////

	std::cout << std::endl << "--CPU--" << std::endl;

	// 入出力構造体インスタンス化&初期化
	for (int i = 0; i < num_loop; i++) {
		input.a[i] = i;
		output.b[i] = 0.0f;
	}

	// 計算
	std::cout << "計算中" << std::endl;
	QueryPerformanceCounter(&start);
	cpu::run<INPUT_DATA, OUTPUT_DATA>(num_loop, &input, &output);
	QueryPerformanceCounter(&end);

	// 結果出力
	for (int i = 0; i < num_loop; i++) {
		if (i >= 10) break;
		std::cout << output.b[i] << std::endl;
	}

	// 所要時間
	std::cout << "所要時間" << std::fixed << (double)(end.QuadPart - start.QuadPart) / freq.QuadPart << "sec.\n";
	std::cout << num_loop << "/48000=" << (double)(num_loop) / 48000.0 << std::endl;
	std::cout <<
		"残った時間:" <<
		((double)(num_loop) / 48000.0) - ((double)(end.QuadPart - start.QuadPart) / freq.QuadPart) <<
		"sec." <<
		std::endl;
	std::cout <<
		"上記の内の何%を処理に要したか:" <<
		(100.0 * 48000.0 * (double)(end.QuadPart - start.QuadPart)) / ((double)(num_loop)* freq.QuadPart) <<
		"%" <<
		std::endl;

	/////////////////////////////////////////////////////////////////////////////////////////////////

	// 入力があるまで待機
	//std::getchar();

	// 処理の終了
	gpu::exitGL();
	return;
}