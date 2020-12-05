#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D waterTexture;

// elapsed time
uniform float elapsedTime;

void main()
{
	float newX= ((int((TexCoord.x + elapsedTime/3) * 100) % 100) / 100.0);
	vec2 transformedUV = vec2(newX, TexCoord.y);
	vec4 tex_color = texture(waterTexture, transformedUV);
	tex_color.a = 0.6;
	FragColor = tex_color;
}