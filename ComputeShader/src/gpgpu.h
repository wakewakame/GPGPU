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
		GLuint SSBO_IN; // 入力SSBOオブジェクトのID
		GLuint SSBO_OUT; // 出力SSBOオブジェクトのID
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
			/*
			// シングルクォーテーション、ダブルクォーテーション内は無視

			// コメントアウトの除去

			// シェーダのバージョン指定
			*SorceCode = "#version 430\n" + *SorceCode;
			*/

			/*
			コメントアウト除去->コメント内の\n以外の改行コード除去->for文で文字列文ループ->"",(),{}などの囲いを見つけるごとにTree形成
			インデントやスペースはそのままにしておく
			Treeのノードには、ほかの"",(),{}などのノード、それ以外を,や;で区切ったテキストノード、自ノードの"",(),{},,,;などの属性の情報を保持させる
			""内は処理しない \" に注意
			()内は,と;で区切る
			{}内は通常通りに処理

			INPUT_DATAやOUTPUT_DATA,SLEDのノードを探し、ツリーを置換
			ツリーを文字列に再形成
			*/

			return;
		}

		/*
		コンピュートシェーダー読み込み関数
			引数1:コンピュートシェーダのソースコードのファイルパス
			戻り値:成功すれば1
		*/
		bool loadShader(std::string Filename)
		{
			// 変数の値の初期化
			FID = GL_FALSE;
			SSBO_IN = GL_FALSE;
			SSBO_OUT = GL_FALSE;

			// SSBOの生成
			if (typeid(INPUT) != typeid(void)) // GPUに送るデータがあるか確認
			{
				glGenBuffers(1, &SSBO_IN); // 空の入力SSBOオブジェクト生成(第一引数は生成するオブジェクトの個数)
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_IN); // 生成した入力SSBOオブジェクトのバインド
				glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(INPUT), NULL, GL_STATIC_DRAW); // 入力SSBOオブジェクトのバッファ確保
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_IN, SSBO_IN); // GPUのRAMと紐ずけ紐づけ(Bind Pointは重複を避けるためにSSBOのIDを使用)
			}
			if (typeid(OUTPUT) != typeid(void)) // GPUから受け取るデータがあるか確認
			{
				glGenBuffers(1, &SSBO_OUT); // 空の出力SSBOオブジェクト生成(第一引数は生成するオブジェクトの個数)
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_OUT); // 生成した入力SSBOオブジェクトのバインド
				glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(OUTPUT), NULL, GL_STATIC_DRAW); // 入力SSBOオブジェクトのバッファ確保
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_OUT, SSBO_OUT); // GPUのRAMと紐ずけ紐づけ(Bind Pointは重複を避けるためにSSBOのIDを使用)
			}
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			/*
			SSBOに関する参考URL
			http://techblog.sega.jp/entry/2016/10/27/140454
			http://hmgmmsa.hatenablog.com/entry/2017/05/05/041200
			http://www.geeks3d.com/20140704/tutorial-introduction-to-opengl-4-3-shader-storage-buffers-objects-ssbo-demo/
			このへん

			SSBOのバッファ確保は
			glBufferData
			SSBOにデータ代入は
			glBufferSubData
			glBindBufferBase
			このへん？
			*/

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

			// 処理の終了
			return 1;
		}

	public:
		func(std::string Filename)
		{
			// 変数の値の初期化
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
			// シェーダーの指定
			glUseProgram(FID);

			// 値の代入
			if ((typeid(INPUT) != typeid(void)) && (input != nullptr)) // GPUに送るデータがあるか確認
			{
				// SSBO_INバインド
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_IN);
				// バインディングポイントの指定
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBO_IN); // Binding Pointは重複を避けるためにSSBOのIDを使用
				// 構造体データ代入用のメモリ確保
				GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
				// 代入
				std::memcpy(p, input, sizeof(INPUT));
				// 代入した値をGPUに転送&生成したメモリの開放
				glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			}

			// 計算
			glDispatchCompute(512 / 100, 1, 1); // 使用するスレッド数

			// 計算結果の取得
			if ((typeid(OUTPUT) != typeid(void)) && (output != nullptr)) // GPUから受け取るデータがあるか確認
			{
				// SSBO_OUTバインド
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_OUT);
				// バインディングポイントの指定
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO_OUT); // Binding Pointは重複を避けるためにSSBOのIDを使用
				// 構造体データ代入用のメモリ確保&値の取得
				GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
				// 代入
				std::memcpy(output, p, sizeof(OUTPUT));
				// 生成したメモリの開放
				glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			}
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