#version 430

/*
自分用リファレンス

SSBO構造体宣言
XXX: 読み込み専用->readonly 書き込み専用->writeonly
layout(std430, binding=任意のバインディングポイント) XXX 構造体名 {
	構造体の中身
} 変数名
*/

layout(std430,binding=2) buffer OUTPUT_DATA {
	float b[100];
} output_data;

layout(std430,binding=3) buffer OUTPUT_DATA2 {
	float b[100];
} output_data2;

uint index;

layout(local_size_x = 1) in; // 1スレッドで何回処理するか

void main()
{
	index = gl_GlobalInvocationID.x;
	output_data.b[index] = output_data2.b[index];
}