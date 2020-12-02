#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D waterTexture;

// elapsed time
uniform float elapsedTime;

void main()
{
	float newY = ((int((TexCoord.y + elapsedTime) * 100) % 100) / 100.0);
	vec2 transformedUV = vec2(TexCoord.x, newY);
	FragColor = texture(waterTexture, transformedUV);
}