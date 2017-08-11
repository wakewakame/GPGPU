#version 430

/*
�����p���t�@�����X

SSBO�\���̐錾
XXX: �ǂݍ��ݐ�p->readonly �������ݐ�p->writeonly
layout(std430, binding=�C�ӂ̃o�C���f�B���O�|�C���g) XXX �\���̖� {
	�\���̂̒��g
} �ϐ���
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