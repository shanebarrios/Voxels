#version 460 core

layout (location = 0) in uint a_Data;

uniform ivec3 u_Position = ivec3(0);

void main()
{
	ivec3 chunkOffset = ivec3
	(
		a_Data & 0x1Fu,
		(a_Data >> 5u) & 0x1Fu, 
		(a_Data >> 10u) & 0x1Fu
	);

	uint textureIndex = (a_Data >> 15u) & 0xFFu;

	// Quick hack to prevent water from casting shadows
	const uint waterIndex = 8;
	if (textureIndex == waterIndex)
	{
		gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		return;
	}

	vec4 worldPosition = vec4(u_Position + chunkOffset, 1.0);

	gl_Position = worldPosition;
}