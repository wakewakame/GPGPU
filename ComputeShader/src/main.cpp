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
	gpu::func<INPUT_DATA,OUTPUT_DATA> func1(Filename);

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

	// 計算
	INPUT_DATA input;
	OUTPUT_DATA output;
	error = func1.Compute(&input, &output);

	// 入力があるまで待機
	std::getchar();

	// 処理の終了
	gpu::exitGL();
	return;
}