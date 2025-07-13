#version 460 core

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 v_TexCoords[];

out vec2 g_TexCoords;

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
	mat4 u_LightSpace[4];
	mat4 u_NormalTransform;
};

void main()
{
	for (int i = 0; i < 3; i++)
	{
		g_TexCoords = v_TexCoords[i];
		gl_Position = u_LightSpace[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}
