#version 460 core

out float FragColor;

in vec2 v_TexCoords;

uniform sampler2D u_Texture;

void main()
{
	vec2 texelSize = 1.0 / textureSize(u_Texture, 0);
	float result = 0.0;
	for (int x = -2; x <= 2; x++)
	{
		for (int y = -2; y <= 2; y++)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(u_Texture, v_TexCoords + offset).r;
		}
	}
	FragColor = result / 25.0;
}