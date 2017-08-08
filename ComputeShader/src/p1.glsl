#version 430

struct Point{
    vec2 position;
    vec2 velocity;
};
layout(std430) buffer Points
{
    Point ins[];
} point;
uniform ivec2 texSize;
uniform vec2 screenSize;
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main()
{
    uint index = gl_WorkGroupSize.x * gl_NumWorkGroups.x * gl_GlobalInvocationID.y + gl_GlobalInvocationID.x;
    Point pos;
    pos.position.xy = vec2(index % texSize.x, index / texSize.x) + screenSize.xy / 2 - texSize / 2;
    pos.velocity = vec2(0);
    point.ins[index] = pos;
}