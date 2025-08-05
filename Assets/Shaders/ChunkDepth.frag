#version 330 core

uniform sampler2D u_TextureAtlas;

in vec2 v_TexCoords;

void main()
{
	float a = texture(u_TextureAtlas, v_TexCoords).a;
	if (a < 0.05) discard;
}