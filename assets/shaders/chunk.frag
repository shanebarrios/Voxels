#version 460 core

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
	mat4 u_LightSpace[4];
};

in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 FragColor;

uniform sampler2D u_TextureAtlas;
uniform sampler2DArray u_ShadowMap;

uniform vec4 u_SubfrustaPlanes;

const vec3 k_LightDir = vec3(-1.0f, -1.0f, -1.0f);
const vec3 k_LightColor = vec3(1.0, 1.0, 0.8);
const float k_AmbientFactor = 0.2;
const float k_DiffuseFactor = 0.8;

float ShadowCalculation()
{
	vec4 fragPosViewSpace = u_View * vec4(v_FragPos, 1.0);
	float viewDepth = -fragPosViewSpace.z;

	int layer = 3;
	for (int i = 0; i < 3; i++)
	{
		if (viewDepth < u_SubfrustaPlanes[i])
		{
			layer = i;
			break;
		}
	}

	vec4 fragPosLightSpace = u_LightSpace[layer] * vec4(v_FragPos, 1.0);

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = (projCoords + 1.0) / 2.0;

	vec2 texelSize = 1.0 / vec2(textureSize(u_ShadowMap, 0));
	float currentDepth = projCoords.z;

	float shadow = 0.0;
	float bias = max(0.05 * (1.0 - dot(v_Normal, k_LightDir)), 0.005);  

	bias /= (u_SubfrustaPlanes[layer] * 0.5);
	for (int x = -2; x <= 2; x++)
	{
		for (int y = -2; y <= 2; y++)
		{
			float pcfDepth = texture(u_ShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow /= 25.0;

	if (projCoords.z > 1.0) return 0.0;
	return shadow;
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