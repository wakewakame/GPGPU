# GPGPUを楽にするクラス  
本体は

'''
ComputeShader/src/gpgpu.h
'''

実行方法は

'''
1.NupenGLをプロジェクトに追加
2.実行
3.p1.glslを選択
4....(未実装)
'''

以下 Qiitaに投稿予定の文
  
# ※まだ文をしっかりまとめてない  

ロクに知識もないクセに見栄張ってる奴が、GPGPUみたいなちょっと難しいことしようとすると、英語の資料しか見当たらず絶望します。  
  
そうです私の事です。  
  
ただ並列処理したいだけなのに、よくわからない処理いっぱいしなきゃいけない萎える  
と、毎回嘆いているので、いい加減それらを簡単にするクラスを作ろうと思い、作りました。  
  
```cpp:CSM.cpp  
工事中  
```  
  
```cpp:main.cpp  
工事中  
```  
  
```cpp:program.compute  
工事中  
```  
  
  
#注意点  
すでにGLFW,GLEWを使用しているプログラム内でこのクラスを使用する場合、Init関数は呼ばなくて大丈夫です。  
GL_SHADER_STORAGE_BUFFERのbind pointを勝手に追加しています。  
bind pointはSSBOのindex番号と同じですので、クラス外でglBindBufferBaseを使用する場合はbind pointが重複しないように気を付けてください。  
  
#あとがき  
私がGPGPU使いたいときに、パッと実装できるようにするため、コメントだらけにしました。  
ウザい程にコメントを書いたので、多分消した方が可読性が上がります。  
  
修飾子とかいろいろわかりづらかったので、並列処理に使う言語はGLSLをかなり抽象化したみたいなものにしました。  
「画像とかは扱えねーじゃんウゼェ!」  
お気持ちはわかりますが、画像を読み込む実装ができるならこんなクラス使わず自分で実装しましょう。  
  
あと、GLSLにコードを変換する際、ただ指定文字を置換しているだけなので、バグだらけです。。。  
本当は/**/や""の中の文字は置換しちゃダメだとか、いろいろやらなきゃいけないのですが、面倒だったので、、、  
そのうち実装していくつもりです。  
  
趣味でプログラミングしてる人なので、本職の人から見ると悲惨なコードかもしれません。  
ご了承願いたい。  
  
#参考URL  
OpenGLに関する記事  
[OpenGL (4.3) compute shader example](http://wili.cc/blog/opengl-cs.html "OpenGL (4.3) compute shader example")  
[OpenGL® 4.5 Reference Pages](https://www.khronos.org/registry/OpenGL-Refpages/gl4/ "OpenGL® 4.5 Reference Pages")  
[opengl-tutorial](http://www.opengl-tutorial.org/ "opengl-tutorial")  
[OpenGL Wiki](https://www.khronos.org/opengl/wiki/ "OpenGL Wiki")  
[モダンな OpenGL で頂点モーフ](http://techblog.sega.jp/entry/2016/10/27/140454 "モダンな OpenGL で頂点モーフ")  
  
テキストファイルの読み込みに関する記事  
[C++でファイル読込み パターン別まとめ](http://fa11enprince.hatenablog.com/entry/2014/04/03/233500 "C++でファイル読込み パターン別まとめ")  
  
ファイル選択ダイアログの表示に関する記事  
[ファイルを開くダイアログ - GetOpenFileName](http://www.geocities.co.jp/Playtown-Dice/9391/program/win05.html "ファイルを開くダイアログ - GetOpenFileName")  
