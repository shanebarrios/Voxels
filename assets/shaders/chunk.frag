#version 330 core

in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec4 v_FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D u_TextureAtlas;
uniform sampler2D u_ShadowMap;

const vec3 k_LightDir = vec3(2.0f, -4.0f, 1.0f);
const vec3 k_LightColor = vec3(1.0, 1.0, 0.8);
const float k_AmbientFactor = 0.4;
const float k_DiffuseFactor = 0.9;

float ShadowCalculation()
{
	vec3 projCoords = v_FragPosLightSpace.xyz / v_FragPosLightSpace.w;
	projCoords = (projCoords + 1.0) / 2.0;
	float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	return currentDepth > closestDepth ? 1.0 : 0.0;
}

void main()
{
	vec4 texColor = texture(u_TextureAtlas, v_TexCoords);
	if (texColor.a < 0.001) discard;

	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(-k_LightDir);

	vec3 ambient = k_AmbientFactor * vec3(texColor);

	float diff = max(dot(normal, lightDir), 0.0) * k_DiffuseFactor;
	vec3 diffuse = diff * vec3(texColor);

	float shadow = ShadowCalculation();

	FragColor = vec4(ambient + (1.0 - shadow) * diffuse, texColor.a);
}