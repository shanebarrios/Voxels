#version 460 core

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
	mat4 u_LightSpace[4];
};

void main()
{
	for (int i = 0; i < 3; i++)
	{
		gl_Position = u_LightSpace[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}
