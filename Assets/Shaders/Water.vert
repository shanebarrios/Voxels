#version 330 core

layout (location = 0) in uint a_Data;

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
	mat4 u_LightSpace[4];
};

uniform ivec3 u_Position = ivec3(0);

out vec2 v_TexCoords;
out vec3 v_Normal;
out vec3 v_FragPos;

const vec3 k_FaceNormals[6] = vec3[]
(
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.0, -1.0),
	vec3(-1.0, 0.0, 0.0),
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, -1.0, 0.0)
);

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
	uint face = (a_Data >> 28u) & 0x7u;

	v_Normal = k_FaceNormals[face];

	v_TexCoords = vec2
	(
		((textureIndex & 0xFu) + u) / 16.0,
		(15u - (textureIndex >> 4u) + v) / 16.0
	);

	v_FragPos = u_Position + chunkOffset;

	vec4 worldPosition = vec4(u_Position + chunkOffset, 1.0);

	gl_Position = u_Projection * u_View * worldPosition;
}