#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <cmath>
#include "transform.h"

// GPGPUに必要な関数やクラスをまとめたもの
namespace gpgpu
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

	// 空の構造体宣言
	struct Empty{};

	/*
	GPU側に変数を追加するクラス
	インスタンスを生成するごとに、GPU側に変数が追加される
	dataはテンプレート引数の型で生成される変数(TYPEを継承しようかとも思ったが、メンバ名の重複などが厄介なのでやめた)
	dataの内容はset,get関数でCPU,GPUの変数を同期する
	set関数でGPU側に変数を転送し、get関数でGPU側の変数を取得する
		テンプレート引数1:GPU側に生成する構造体の型
	*/
	template <typename TYPE>
	class var
	{
	private:
		GLuint SSBO; // SSBOオブジェクトのID
		unsigned int size; // SSBOオブジェクトのサイズ

		/*
		SSBO生成関数
			戻り値:成功すれば1
		*/
		bool createSSBO()
		{
			deleteSSBO(); // SSBO初期化
			glGenBuffers(1, &SSBO); // 空の出力SSBOオブジェクト生成(第一引数は生成するオブジェクトの個数)
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO); // 生成した入力SSBOオブジェクトのバインド
			glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_STATIC_DRAW); // 入力SSBOオブジェクトのバッファ確保
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO, SSBO); // GPUのRAMと紐ずけ紐づけ(Bind Pointは重複を避けるためにSSBOのIDを使用)

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			return 1;
		}

		/*
		SSBO破棄関数
		*/
		void deleteSSBO()
		{
			if (SSBO != GL_FALSE) {
				glDeleteBuffers(1, &SSBO);
				SSBO = GL_FALSE;
			}
		}

	public:
		// 指定された型の構造体生成
		TYPE data;

		/*
		コンストラクタ
		クラスのテンプレート引数に取る型に可変長構造体などが入る場合、
		コンストラクタの引数に任意の型のサイズを指定可能
			引数1:型のサイズ(省略可能)
		*/
		var(unsigned int set_size = 0)
		{
			// 変数初期化
			SSBO = GL_FALSE;
			// サイズ取得
			if (set_size != 0)
			{
				size = set_size;
			}
			else
			{
				size = sizeof(TYPE);
			}
			// SSBO生成
			createSSBO();
		}

		// デストラクタ
		~var()
		{
			deleteSSBO();
		}

		// GPU側に変数の内容を転送
		void set()
		{
			// SSBOバインド
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
			// 構造体データ代入用のメモリ確保
			GLvoid* p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			// 代入
			std::memcpy(p, &data, size);
			// 代入した値をGPUに転送&生成したメモリの開放
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			// SSBOアンバインド
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
		void set(TYPE *set_data)
		{
			// SSBOバインド
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
			// 構造体データ代入用のメモリ確保
			GLvoid* p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			// 代入
			std::memcpy(p, &set_data, size);
			// 代入した値をGPUに転送&生成したメモリの開放
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			// SSBOアンバインド
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		// GPU側の変数を取得
		void get()
		{
			// SSBO_OUTバインド
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
			// 構造体データ代入用のメモリ確保&値の取得
			GLvoid* p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT);
			// 代入
			std::memcpy(&data, p, size);
			// 生成したメモリの開放
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			// SSBOアンバインド
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		// SSBO ID取得
		GLuint ID() const {
			return SSBO;
		}
	};

	/*
	GPU側に関数を追加するクラス
	インスタンスを生成するごとに、GPUに関数が追加される
	Compute関数でGPU側の関数を実行する
	エラーはgetErrorで取得できる
	*/
	class func
	{
	private:

		Code code; // シェーダーのソースコード
		GLuint FID; // GPU側の関数ID
		std::vector<GLuint> SSBO_back; // 前回のGPU側の変数ID
		std::vector<GLuint> SSBO; // GPU側の変数ID
		uvec3 loop; // ループ回数
		uvec3 sled; // 生成するスレッド数
		std::string CSCode; // GLSLコード
		bool error; // エラーが発生すれば1が代入される
		std::string error_desc; // エラー文代入用変数
		bool compile; // 再コンパイルフラグ

		/*
		コンピュートシェーダー読み込み関数
			引数1:コンピュートシェーダのソースコードのファイルパス
			戻り値:成功すれば1
		*/
		bool loadShader()
		{
			// コンピュートシェーダーの破棄
			deleteShader();

			// ソースコードのコンパイル
			GLuint CSID = glCreateShader(GL_COMPUTE_SHADER); // 空のコンピュートシェーダー生成
			CSCode = code.get_glsl(); // ソースコードをコンピュートシェーダに変換
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

			// コンパイルフラグの回収
			compile = 0;

			// 処理の終了
			return 1;
		}

		// コンピュートシェーダー破棄関数
		void deleteShader() {
			// シェーダーが存在しているか確認
			if (FID != GL_FALSE) {
				// シェーダーの開放
				glDeleteShader(FID);
				FID = GL_FALSE;
			}
		}
		// Compute関数の可変長引数取得関数
		void getArg() // 引数の数が0になったとき
		{
			// 前回の引数IDと違う場合、シェーダーの再コンパイル
			if(SSBO_back != SSBO)
			{
				SSBO = SSBO_back;
				code.set_param("430", loop, sled, (std::vector<unsigned int>)SSBO);
				compile = 1;
			}
			SSBO_back.clear();
		}
		// SSBO_backに引数を1つずつ再起処理で追加
		template<typename First, typename ...Rest>
		void getArg(First const &first, Rest const &...rest)
		{
			SSBO_back.push_back(first.ID());
			getArg(rest...);
		}

	public:
		func(std::string Filename)
		{
			// 変数の値の初期化
			error = 0;
			error_desc = "";
			FID = GL_FALSE;
			if (!code.open(Filename, 1))
			{
				error = 1;
				error_desc = "File is not found"; // エラー文代入
			}
			compile = 1;
		}
		func(std::string Filename, unsigned int set_loop, unsigned int set_sled)
		{
			// 変数の値の初期化
			error = 0;
			error_desc = "";
			loop.x = set_loop;
			if (set_sled > set_loop) sled.x = set_loop;
			else sled.x = set_sled;
			FID = GL_FALSE;
			if (!code.open(Filename, 1))
			{
				error = 1;
				error_desc = "File is not found"; // エラー文代入
			}
			compile = 1;
		}

		~func()
		{
			deleteShader();
		}

		/*
		並列処理実行関数
		注意:入出力の構造体の中にポインタがあると、GPU側で値を参照できなくなるのでNGだけど、固定長の生配列とかならOK
			戻り値:成功すれば1
		*/
		template<typename ...ARG>
		bool Compute(ARG const &...arg)
		{
			// 引数の取得
			getArg(arg...);

			// 再コンパイルの必要があれば、再コンパイル
			if (compile) loadShader();

			// シェーダーの指定
			glUseProgram(FID);

			// 計算
			glDispatchCompute(sled.x, sled.y, sled.z); // 使用するスレッド数(x, y, z)

			return 1;
		}

		/*
		ループ回数と使用するスレッド数指定
			引数1,3,5:x,y,zでループさせる回数(2項目以降は省略可)
			引数2,4,6:x,y,zで使用するスレッド数(2項目以降は省略可)
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
			// 代入
			loop.x = set_loop_x;
			if (set_sled_x > set_loop_x) sled.x = set_loop_x;
			else sled.x = set_sled_x;
			loop.y = set_loop_y;
			if (set_sled_y > set_loop_y) sled.y = set_loop_y;
			else sled.y = set_sled_y;
			loop.z = set_loop_z;
			if (set_sled_z > set_loop_z) sled.z = set_loop_z;
			else sled.z = set_sled_z;
			// 再コンパイルフラグ
			compile = 1;
		}

		/*
		コードにdefine追加関数
			引数1:定数名
			引数2:定数値
		*/
		void set_def(std::string def_name, std::string def_val)
		{
			code.set_define(def_name, def_val);
			compile = 1;
		}

		// コード確認
		std::string getCode()
		{
			return CSCode;
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