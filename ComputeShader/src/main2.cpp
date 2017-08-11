#include "gpgpu.h"
#include <iostream>
#include <windows.h>

struct INPUT_DATA
{
	int a[10000];
};

struct OUTPUT_DATA
{
	float b[10000];
};

void main()
{
	/*
	主な流れ
	1.インスタンス生成
	2.初期化
	3.プログラムの生成
	4.計算
	*/

	// エラー確認用変数の宣言
	bool error;

	// GLの初期化
	std::cout << "OpenGLを初期化しています" << std::endl;
	error = gpu::initGL();
	
	// エラー確認
	if (!error)
	{
		std::cout << "OpenGLの初期化に失敗しました" << std::endl;
		std::getchar();
		return;
	}

	// GPU側に追加する関数のソースコードのファイルパス指定
	std::cout << "GPU側に追加する関数のソースコードのファイルパスを指定します" << std::endl;
		// ファイル選択ダイアログの表示
		OPENFILENAME ofn;
		char szFile[MAX_PATH] = "";
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.lpstrFilter = TEXT("全てのファイル(*.*)\0*.*\0");
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn) == 0)
		{
			std::cout << "ファイルが選択されませんでした" << std::endl;
			std::getchar();
			return;
		}

		// 選択されたファイルのパス取得
		std::string Filename = ofn.lpstrFile;
		std::cout << Filename << std::endl;

	// GPU側に関数を追加
	std::cout << "関数を追加しています" << std::endl;
	gpu::func<INPUT_DATA, gpu::Empty> func1(Filename);

	// エラー確認
	if (func1.checkError() == 1)
	{
		std::cout << "エラー\n" << func1.getError() << std::endl;
		std::getchar();
		return;
	}

	// 生成したプログラムのID表示
	std::cout << "完了" << std::endl;
	std::cout << "プログラムID:" << func1.getPID() << std::endl;


	///
	LARGE_INTEGER freq;
	if (!QueryPerformanceFrequency(&freq)) return;
	LARGE_INTEGER start, end;
	///


	/////////////////////////////////////////////////////////////////////////////////////////////////

	std::cout << std::endl << "--GPU--" << std::endl;

	// 入出力構造体インスタンス化&初期化
	std::cout << "変数初期化中" << std::endl;
	INPUT_DATA input;
	OUTPUT_DATA output;
	for (int i = 0; i < 10000; i++) {
		input.a[i] = i;
		output.b[i] = 0.0f;
	}

	// 計算
	std::cout << "計算中" << std::endl;
	QueryPerformanceCounter(&start);
	error = func1.Compute(&input, nullptr);
	QueryPerformanceCounter(&end);

	// エラー確認
	if (error == 0)
	{
		std::cout << "エラー\n" << func1.getError() << std::endl;
		std::getchar();
		return;
	}

	// 計算結果の表示
	///*
	for (int i = 0; i < 10; i++) {
		std::cout << input.a[i] << ":" << std::fixed << output.b[i] << std::endl;
	}
	//*/

	// 所要時間
	std::cout << "所要時間" << std::fixed << (double)(end.QuadPart - start.QuadPart) / freq.QuadPart << "sec.\n";

	/////////////////////////////////////////////////////////////////////////////////////////////////

	std::cout << std::endl << "--CPU--" << std::endl;

	// 入出力構造体初期化
	std::cout << "変数初期化中" << std::endl;
	for (int i = 0; i < 10000; i++) {
		input.a[i] = i;
		output.b[i] = 0.0f;
	}

	// 計算
	std::cout << "計算中" << std::endl;
	QueryPerformanceCounter(&start);
	for (int i = 0; i < 10000; i++) {
		output.b[i] = sin((float)input.a[i]);
	}
	QueryPerformanceCounter(&end);

	// 計算結果の表示
	/*
	for (int i = 0; i < 1000; i++) {
		std::cout << input.a[i] << ":" << std::fixed << output.b[i] << std::endl;
	}
	*/

	// 所要時間
	std::cout << "所要時間" << std::fixed << (double)(end.QuadPart - start.QuadPart) / freq.QuadPart << "sec.\n";

	/////////////////////////////////////////////////////////////////////////////////////////////////

	// 入力があるまで待機
	std::getchar();

	// 処理の終了
	gpu::exitGL();
	return;
}