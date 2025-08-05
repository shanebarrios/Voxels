#version 330 core

layout (location = 0) in uint a_Data;

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
	mat4 u_LightSpace[4];
	mat4 u_NormalTransform;
};

uniform uint u_CascadeIndex;
uniform ivec3 u_Position = ivec3(0);

out vec2 v_TexCoords;

void main()
{
	ivec3 chunkOffset = ivec3
	(
		a_Data & 0x3Fu,
		(a_Data >> 6u) & 0x3Fu, 
		(a_Data >> 12u) & 0x3Fu
	);

	uint textureIndex = (a_Data >> 18u) & 0xFFu;
	float u = (a_Data >> 26u) & 0x1u;
	float v = (a_Data >> 27u) & 0x1u;

	v_TexCoords = vec2(
		((textureIndex & 0xFu) + u) / 16.0,
		(15u - (textureIndex >> 4u) + v) / 16.0
	);

	vec4 worldPosition = vec4(u_Position + chunkOffset, 1.0);

	gl_Position = u_LightSpace[u_CascadeIndex] * worldPosition;
}