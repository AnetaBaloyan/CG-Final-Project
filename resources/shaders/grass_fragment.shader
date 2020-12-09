#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D grassDiffuse;


void main()
{
	vec4 col = texture(grassDiffuse, TexCoord);

	if (col.a < 0.001) {
		discard;
	}
	FragColor = col;

}