#version 460 core

in vec2 g_TexCoords;

uniform sampler2D v_TextureAtlas;

out vec4 FragColor;

void main()
{
	vec4 tex = texture(v_TextureAtlas, g_TexCoords);
	if (tex.a < 0.95) discard;
	FragColor = vec4(0.8);
}