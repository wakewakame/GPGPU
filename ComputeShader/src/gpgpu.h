#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iterator>

// GPGPUに必要な関数やクラスをまとめたもの
namespace gpu
{
	/*
	初期化関数
	GLFW,GLEWが初期化されていなければ、この関数で初期化する
		戻り値:成功すれば1
	*/
	bool initGL()
	{
		glewExperimental = GL_TRUE; // よくわかんないけど必要らしい
		if (glfwInit() == GL_FALSE) // GLFW初期化&エラーチェック
		{
			return 0;
		}
		// OpenGL Version 4.3を指定
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_VISIBLE, 0); // 描画処理はしないので、ウィンドウは生成しないようにする
		GLFWwindow* win = glfwCreateWindow(640, 480, "", NULL, NULL); // ウィンドウ生成
		glfwMakeContextCurrent(win); // 生成したウィンドウをOpenGLの処理対象にする
		if (glewInit() != GLEW_OK) // GLEW初期化&エラーチェック
		{
			return 0;
		}
		return 1;
	}

	/*
	終了処理関数
	initGLを使用した場合、プログラムの終了時にこれを呼ぶ
	*/
	void exitGL()
	{
		glfwTerminate(); // GLFWの終了
	}

	/*
	GPU側に関数を追加するクラス
	インスタンスを生成するごとに、GPUに関数が追加される
	Compute関数でGPU側の関数を実行する
	エラーはgetErrorで取得できる
		テンプレート引数1:GPU側に値を渡すための入力構造体(無ければvoid型を指定)
		テンプレート引数2:GPU側から値を受け取るための出力構造体(無ければvoid型を指定)
	*/
	template <typename INPUT, typename OUTPUT>
	class func
	{
	private:
		GLuint FID; // GPU側の関数ID
		GLuint SSBO; // SSBOオブジェクトのID
		bool error; // エラーが発生すれば1が代入される
		std::string error_desc; // エラー文代入用変数

		/*
		テキストファイル読み込み関数
			引数1:テキストのファイルのパス
			引数2:読み込んだテキストを代入する変数
			戻り値:成功すれば1
		*/
		bool loadText(std::string Filename, std::string *Text)
		{
			*Text = ""; // テキスト初期化
			std::ifstream ifsText(Filename); // ifstreamでテキスト読み込み
			if (ifsText.fail()) // 成功確認
			{
				error = 1;
				error_desc = "File Open Failed";
				return 0;
			}
			// 使い方とか仕組みがよくわからないけど、これで読み込めるのでOK。
			std::istreambuf_iterator<char> it(ifsText);
			std::istreambuf_iterator<char> last;
			std::string _Text(it, last);
			*Text = _Text; // 代入
			return 1;
		}

		/*
		コンピュートシェーダーに変換する関数
			引数1:変換するテキスト
			戻り値:変換されたテキスト
		*/
		void Translation(std::string *SorceCode)
		{
			// シェーダのバージョン指定
			//*SorceCode = "#version 430\n" + *SorceCode;

			return;
		}

		/*
		コンピュートシェーダー読み込み関数
			引数1:コンピュートシェーダのソースコードのファイルパス
			戻り値:成功すれば1
		*/
		bool loadShader(std::string Filename)
		{
			// ソースコードのコンパイル
			GLuint CSID = glCreateShader(GL_COMPUTE_SHADER); // 空のコンピュートシェーダー生成
			std::string CSCode; // ソースコードの文字列代入用変数
			if (!loadText(Filename, &CSCode)) // ソースコードの読み込み&エラー確認
			{
				return 0;
			}

			// コンピュートシェーダに変換
			Translation(&CSCode);
			char const *charCSCode = CSCode.c_str(); // char型にキャスト
			glShaderSource(CSID, 1, &charCSCode, NULL); // OpenGL側にソースコードを送る
			glCompileShader(CSID); // ソースコードのコンパイル

			// エラー確認
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
				return 0; // GL_FALSEを返す
			}

			// プログラムの生成
			GLuint CSPID = glCreateProgram(); // 空のプログラム生成
			glAttachShader(CSPID, CSID); // 生成したプログラムに、コンパイルしたシェーダーをアタッチ
			glLinkProgram(CSPID); // 生成したプログラムをリンク

			// エラー確認
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
				return 0; // GL_FALSEを返す
			}

			// 生成したプログラムのIDを代入
			FID = CSPID;

			// SSBOの生成
			glGenBuffers(1, &SSBO); // 空のSSBOオブジェクト生成(第一引数は生成するオブジェクトの個数)
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO); // 生成したSSBOオブジェクトのバインド
			//glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, buffer, GL_STATIC_DRAW);
			/*
			SSBOに関する参考URL
			http://techblog.sega.jp/entry/2016/10/27/140454
			http://hmgmmsa.hatenablog.com/entry/2017/05/05/041200
			このへん
			*/

			// 処理の終了
			return 1;
		}

	public:
		func(std::string Filename)
		{
			// 変数の値の初期化
			FID = GL_FALSE;
			SSBO = GL_FALSE;
			error = 0;
			error_desc = "";
			loadShader(Filename);
		}

		~func()
		{

		}

		/*
		並列処理実行関数
		注意:入出力の構造体の中にポインタがあると、GPU側で値を参照できなくなるのでNGだけど、固定長の生配列とかならOK
			引数1:入力構造体のポインタ(無ければnullptrを指定)
			引数2:出力構造体のポインタ(無ければnullptrを指定)
			戻り値:成功すれば1
		*/
		bool Compute(INPUT *input, OUTPUT *output)
		{
			glUseProgram(FID); // シェーダーの指定
			if ((typeid(INPUT) != typeid(void)) && (input != nullptr)) // GPUに送るデータがあるか確認
			{

			}
			if ((typeid(OUTPUT) != typeid(void)) && (output != nullptr)) // GPUから受け取るデータがあるか確認
			{

			}
			//glUniform1f(glGetUniformLocation(computeHandle, "input"), (float)frame*0.01f);
			//glDispatchCompute(512 / 16, 512 / 16, 1);
			return 1;
		}

		// エラー確認
		bool checkError()
		{
			return error;
		}

		// エラー文取得関数
		std::string getError()
		{
			return error_desc;
		}

		// 生成したコンピュートシェーダーのプログラムID取得関数
		GLuint getPID()
		{
			return FID;
		}
	};
}