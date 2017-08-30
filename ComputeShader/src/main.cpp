#include "../lib/gpgpu.h"
#include <iostream>
#include <cmath>
#include <windows.h>

std::string DIR;
std::string FILENAME;

// GPU側でのループ回数
#define num_loop_x 10
#define num_loop_y 10
// GPU側に何個スレッドを用意するか(多いほうが早い)
#define num_sled_x 10
#define num_sled_y 10

// GPU側と共有する構造体
struct DATA
{
	float arr[num_loop_x][num_loop_y];
};

void main()
{
	// カレントディレクトリ取得
	char DIR_[255];
	GetCurrentDirectory(255, DIR_);
	DIR = std::string(DIR_) + "\\src\\";
	FILENAME = "program.gpu";

	// gpgpu初期化
	gpgpu::initGL();
	if (gpgpu::error != "") // エラー確認
	{
		std::cout << gpgpu::error << std::endl;
		std::getchar();
		return;
	}

	// gpu側に変数を追加
	gpgpu::var<DATA> data;

	// 変数初期化
	for (int i = 0; i < num_loop_x * num_loop_y; i++) {
		data.data->arr[i % num_loop_x][i / num_loop_x] = 0.0f;
	}

	// gpu側に関数を追加
	gpgpu::func func(DIR + FILENAME);

	// 何回ループするかを指定
	func.set_loop(num_loop_x, num_sled_x, num_loop_y, num_sled_y);

	// gpu側に変数を送信
	data.set();

	// 変数を渡して計算
	func.Compute(data);
	if (func.checkError()) // エラー確認
	{
		std::cout << func.getCode() << std::endl;
		std::cout << "エラー\n" << func.getError() << std::endl;
		std::getchar();
		return;
	}

	// gpu側の変数を取得
	data.get();

	// 計算結果表示
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 10; i++) {
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

	// 終了処理
	gpgpu::exitGL();

	// キー入力があるまで待機
	std::getchar();
	return;
}