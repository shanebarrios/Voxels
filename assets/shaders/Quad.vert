#version 460 core

layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_Transform = mat4(1.0);

void main()
{
	gl_Position = u_Transform * vec4(a_Pos, 0.0, 1.0);
	v_TexCoords = a_TexCoords;
}