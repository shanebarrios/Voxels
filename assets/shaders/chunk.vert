#version 330 core

layout (location = 0) in uint a_Data;

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
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
		a_Data & 0x1Fu,
		(a_Data >> 5u) & 0x1Fu, 
		(a_Data >> 10u) & 0x1Fu
	);

	uint textureIndex = (a_Data >> 15u) & 0xFFu;
	uint u = (a_Data >> 23u) & 0x1u;
	uint v = (a_Data >> 24u) & 0x1u;

	uint face = (a_Data >> 25u) & 0x7u;
	v_Normal = k_FaceNormals[face];

	v_TexCoords = vec2
	(
		((textureIndex & 0xFu) + u) / 16.0,
		(15u - (textureIndex >> 4u) + v) / 16.0
	);

	v_FragPos = u_Position + chunkOffset;

	gl_Position = u_Projection * u_View * vec4(u_Position + chunkOffset, 1.0);
}