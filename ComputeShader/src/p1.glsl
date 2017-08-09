layout(std430,binding=1) readonly buffer SSBO {
	vec4 data[];
} INPUT_DATA;

layout(std430,binding=2) writeonly buffer SSBO {
	vec4 data[];
} OUTPUT_DATA;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main()
{
	vec4 data0 = INPUT_DATA.data[0];
	OUTPUT_DATA.data[0] = vec4(0.0);
}