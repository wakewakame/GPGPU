#version 430

/*
�����p���t�@�����X

SSBO�\���̐錾
XXX: �ǂݍ��ݐ�p->readonly �������ݐ�p->writeonly
layout(std430, binding=�C�ӂ̃o�C���f�B���O�|�C���g) XXX �\���̖� {
	�\���̂̒��g
} �ϐ���
*/

layout(std430,binding=2) buffer OUTPUT_DATA {
	float b[100];
} output_data;

layout(std430,binding=3) buffer OUTPUT_DATA2 {
	float b[100];
} output_data2;

uint index;

layout(local_size_x = 1) in; // 1�X���b�h�ŉ��񏈗����邩

void main()
{
	index = gl_GlobalInvocationID.x;
	output_data.b[index] = output_data2.b[index];
}