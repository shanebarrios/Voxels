#version 460 core

uniform sampler2D u_TextureAtlas;

in vec2 g_TexCoords;

void main()
{
	float a = texture(u_TextureAtlas, g_TexCoords).a;
	if (a < 0.05) discard;
}