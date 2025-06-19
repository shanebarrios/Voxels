#version 330 core

in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 FragColor;

uniform sampler2D u_TextureAtlas;

const vec3 k_LightDir = vec3(-0.3, -1.0, -0.2);
const vec3 k_LightColor = vec3(1.0, 1.0, 0.8);
const float k_AmbientFactor = 0.4;
const float k_DiffuseFactor = 0.9;

void main()
{
	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(-k_LightDir);

	vec3 lightColor = vec3(texture(u_TextureAtlas, v_TexCoords));

	vec3 ambient = k_AmbientFactor * lightColor;

	float diff = max(dot(normal, lightDir), 0.0) * k_DiffuseFactor;
	vec3 diffuse = diff * lightColor;

	FragColor = vec4((ambient + diffuse) * k_LightColor, 1.0);
}