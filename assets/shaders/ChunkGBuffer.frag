#version 460 core

layout (location = 0) out vec4 g_Position;
layout (location = 1) out vec4 g_Normal;
layout (location = 2) out vec4 g_Albedo;

in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform sampler2D u_TextureAtlas;

void main()
{
	g_Position = vec4(v_FragPos, 1.0);
	g_Normal = vec4(normalize(v_Normal), 1.0);
	g_Albedo = texture(u_TextureAtlas, v_TexCoords);
	if (g_Albedo.a < 0.05) discard;
}