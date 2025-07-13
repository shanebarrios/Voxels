#version 460 core

in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 FragColor;

uniform sampler2D u_TextureAtlas;

void main()
{
	FragColor = texture(u_TextureAtlas, v_TexCoords);
}