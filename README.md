![demo](image/demo.png)

GPU上で九九を計算する例

# GPGPU
大学1年のころに作成したGPGPUを実現するためのプログラムです。
OpenGLのシェーダーを用いて任意の計算をGPU上で実行できるようにします。
大学4年生の時に実行環境を再現できなくなったため、CMakeで環境構築するように変更しました。
(C++の部分はほとんど変更していません)

# 実行方法

```bash
git clone https://github.com/wakewakame/GPGPU
cd GPGPU
git submodule --update --recursive
mkdir build && cd build
cmake ..
make
./GPGPU
```