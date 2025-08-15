#version 330 core

in vec3 v_SkySpace;

out vec4 FragColor;


const vec3 k_SkyColor = vec3(0.53f, 0.81f, 0.92f);
const vec3 k_SunsetColor = vec3(0.941f, 0.424f, 0.204f);

uniform vec3 u_LightDir;

void main()
{
	vec3 skySpace = normalize(v_SkySpace);

	float cosTheta = dot(-u_LightDir, skySpace);
	vec3 color = mix(k_SkyColor, k_SunsetColor, (cosTheta + 1.0) * 0.5);

	FragColor = vec4(color, 1.0);
}