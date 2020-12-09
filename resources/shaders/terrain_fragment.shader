#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

// texture sampler
uniform sampler2D terrainDiffuse;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float diffuseStrength;


void main()
{
	vec4 tex = texture(terrainDiffuse, TexCoord);

	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, normalize(lightDir)), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	vec3 result = (ambient + diffuse) * tex.rgb;

	FragColor = vec4(result, 1.0f);
}