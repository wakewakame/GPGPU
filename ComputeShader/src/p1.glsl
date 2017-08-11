#version 430

/*
自分用リファレンス

SSBO構造体宣言
XXX: 読み込み専用->readonly 書き込み専用->writeonly
layout(std430, binding=任意のバインディングポイント) XXX 構造体名 {
	構造体の中身
} 変数名
*/

layout(std430,binding=1) readonly buffer INPUT_DATA {
	int a[100];
} input_data;

layout(std430,binding=2) writeonly buffer OUTPUT_DATA {
	float b[100];
} output_data;

uint index;

layout(local_size_x = 100) in;

void main()
{
	index = gl_GlobalInvocationID.x;
	output_data.b[index] = float(input_data.a[index]) + 1.0;
}