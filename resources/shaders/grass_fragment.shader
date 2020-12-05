#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D grassDiffuse;


void main()
{
	FragColor = texture(grassDiffuse, TexCoord);
}