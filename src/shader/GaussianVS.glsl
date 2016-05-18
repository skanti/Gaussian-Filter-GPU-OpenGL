#version 410 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_pos_in;
out vec2 tex_pos;

void main()
{
	tex_pos = tex_pos_in;
    gl_Position = vec4(position,0,1);
}