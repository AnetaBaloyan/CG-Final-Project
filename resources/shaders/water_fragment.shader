#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

// texture sampler
uniform sampler2D waterTexture;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float shininess;
uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;

// elapsed time
uniform float elapsedTime;

void main()
{
	float newX = ((int((TexCoord.x + elapsedTime / 3) * 100) % 100) / 100.0);
	vec2 transformedUV = vec2(newX, TexCoord.y);
	vec4 tex_color = texture(waterTexture, transformedUV);

	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, normalize(lightDir)), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(norm, reflectDir), 0.0), shininess);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * tex_color.rgb;

	FragColor = vec4(result, 0.6);
}