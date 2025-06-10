#version 330 core

layout (location = 0) in ivec3 a_Pos;

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

uniform ivec3 u_Position;

void main()
{
	gl_Position = u_Projection * u_View * vec4(u_Position + a_Pos, 1.0);
	gl_Position.z -= 0.0001;
}