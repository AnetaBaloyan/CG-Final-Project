#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D terrainDiffuse;


void main()
{
	FragColor = texture(terrainDiffuse, TexCoord);
}