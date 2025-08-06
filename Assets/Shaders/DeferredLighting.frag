#version 330 core

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
	mat4 u_LightSpace[4];
};

in vec2 v_TexCoords;

out vec4 FragColor;

uniform sampler2D u_PositionSampler;
uniform sampler2D u_NormalSampler;
uniform sampler2D u_AlbedoSampler;
uniform sampler2DArray u_ShadowMap;

uniform vec4 u_SubfrustaPlanes;
uniform vec3 u_LightDir;

const vec3 k_LightColor = vec3(1.0, 1.0, 0.8);
const float k_AmbientFactor = 0.4;
const float k_DiffuseFactor = 0.8;

float ShadowCalculation(vec3 fragPos)
{
	float viewDepth = -fragPos.z;

	int layer = 3;
	for (int i = 0; i < 3; i++)
	{
		if (viewDepth < u_SubfrustaPlanes[i])
		{
			layer = i;
			break;
		}
	}

	vec4 fragPosLightSpace = u_LightSpace[layer] * inverse(u_View) * vec4(fragPos, 1.0);

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = (projCoords + 1.0) / 2.0;

	vec2 texelSize = 1.0 / vec2(textureSize(u_ShadowMap, 0));
	float currentDepth = projCoords.z;

	float shadow = 0.0;
	float bias = 0.0015 / sqrt(u_SubfrustaPlanes[layer]);
	 
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			float pcfDepth = texture(u_ShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
			shadow += (currentDepth - bias ) > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow /= 9.0;

	if (projCoords.z > 1.0) return 0.0;
	return shadow;
}

void main()
{
	vec3 fragPos = texture(u_PositionSampler, v_TexCoords).rgb;
	vec3 normal = texture(u_NormalSampler, v_TexCoords).rgb;
	vec4 albedoSample = texture(u_AlbedoSampler, v_TexCoords);
	vec3 albedo = albedoSample.rgb;
	float occlusion = albedoSample.a;

	// This is definitely not how you're supposed to do fill color
	if (albedo == vec3(0.0))
	{
		discard;
	};

	vec3 ambient = k_AmbientFactor * albedo * mix(0.4, 1.0, occlusion);

	float diff = max(dot(normal, -u_LightDir), 0.0) * k_DiffuseFactor;
	vec3 diffuse = diff * albedo;
		
	float shadow = ShadowCalculation(fragPos);
	FragColor = vec4(ambient + (1.0 - shadow) * diffuse, 1.0);
}