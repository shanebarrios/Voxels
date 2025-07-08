#version 460 core

in vec2 v_TexCoords;

out vec4 FragColor;

uniform sampler2DArray u_DepthMap;

void main()
{
	float depth = texture(u_DepthMap, vec3(v_TexCoords, 0)).r;
	FragColor = vec4(vec3(depth), 1.0);
}