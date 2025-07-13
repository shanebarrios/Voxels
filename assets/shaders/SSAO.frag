#version 460 core

layout (std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
	mat4 u_LightSpace[4];
};

out float FragColor;

in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D u_PositionSampler;
layout (binding = 1) uniform sampler2D u_NormalSampler;
layout (binding = 2) uniform sampler2D u_NoiseSampler;

uniform vec3 u_Samples[64];

vec2 noiseScale = textureSize(u_PositionSampler, 0) / 4.0;
const float radius = 0.5;
const float bias = 0.025;

void main()
{
	vec3 fragPos = texture(u_PositionSampler, v_TexCoords).rgb;

	if (fragPos == vec3(0.0))
	{
		FragColor = 1.0;
		return;
	}

	vec3 normal = texture(u_NormalSampler, v_TexCoords).rgb;
	vec3 randomVec = texture(u_NoiseSampler, v_TexCoords * noiseScale).rgb;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < 64; i++)
	{
		vec3 samplePos = TBN * u_Samples[i];
		samplePos = fragPos + samplePos * radius;
		vec4 samplePosScreenSpace = u_Projection * vec4(samplePos, 1.0);
		samplePosScreenSpace.xyz /= samplePosScreenSpace.w;
		samplePosScreenSpace.xyz = (samplePosScreenSpace.xyz + 1.0) / 2.0;
		float sampleDepth = texture(u_PositionSampler, samplePosScreenSpace.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / 64);
	FragColor = occlusion;
}