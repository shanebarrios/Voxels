#version 460 core

layout (location = 0) in uint a_Data;

uniform ivec3 u_Position = ivec3(0);

out vec2 v_TexCoords;

void main()
{
	ivec3 chunkOffset = ivec3
	(
		a_Data & 0x1Fu,
		(a_Data >> 5u) & 0x1Fu, 
		(a_Data >> 10u) & 0x1Fu
	);

	uint textureIndex = (a_Data >> 15u) & 0xFFu;
	float u = (a_Data >> 23u) & 0x1u;
	float v = (a_Data >> 24u) & 0x1u;

	vec4 worldPosition = vec4(u_Position + chunkOffset, 1.0);

	v_TexCoords = vec2
	(
		((textureIndex & 0xFu) + u) / 16.0,
		(15u - (textureIndex >> 4u) + v) / 16.0
	);

	gl_Position = worldPosition;
}