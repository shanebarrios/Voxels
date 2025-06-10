#version 330 core

in vec2 v_TexCoords;

out vec4 FragColor;

uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoords);
	if(texColor.a < 0.1) discard;
	FragColor = texColor;
}