#version 330 core

in vec2 v_TexCoords;

uniform sampler2D v_TextureAtlas;

out vec4 FragColor;

void main()
{
	vec4 tex = texture(v_TextureAtlas, v_TexCoords);
	if (tex.a < 0.8) discard;
	gl_FragDepth = gl_FragCoord.z;
}