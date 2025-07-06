#version 330 core

layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

const float k_AspectRatio = 16.0 / 9.0;

void main()
{
	gl_Position = vec4(0.5 * a_Pos.x / k_AspectRatio, 0.5 * a_Pos.y, 0.0, 1.0);
	v_TexCoords = a_TexCoords;
}