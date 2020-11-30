#version 330 core
out vec4 FragColor;

//in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float diffuseStrength;

void main()
{
    /*vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;*/

    FragColor = vec4(255.0, 255.0, 255.0, 1.0);
}