#version 460 core

layout (location = 0) out vec4 g_Position;
layout (location = 1) out vec4 g_Normal;
layout (location = 2) out vec4 g_Albedo;

in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
in float v_AmbientFactor;

uniform sampler2D u_TextureAtlas;

void main()
{
	g_Position = vec4(v_FragPos, 1.0);
	g_Normal = vec4(normalize(v_Normal), 1.0);
	g_Albedo = texture(u_TextureAtlas, v_TexCoords);
	//g_Albedo = vec4(v_AmbientFactor, v_AmbientFactor, v_AmbientFactor, 1.0);
	if (g_Albedo.a < 0.05) discard;
	g_Albedo.a = v_AmbientFactor;
}