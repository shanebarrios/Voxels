#version 330 core

layout(location = 0) in vec2 a_Pos;

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};

out vec3 v_SkySpace;

void main()
{
	mat4 view = u_View;
	view[3] = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 unProject = inverse(u_Projection * view);
	vec4 worldSpace = unProject * vec4(a_Pos, 1.0, 1.0);
	v_SkySpace = worldSpace.xyz / worldSpace.w;
	gl_Position = vec4(a_Pos, 1.0, 1.0);
}