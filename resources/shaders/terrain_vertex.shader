#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 tCoord;

uniform sampler2D heightMap;

void main()
{
    vec4 filt = texture(heightMap, aTexCoord);
    float offset = filt.r / 2;
    vec3 inc = vec3(0.0, -offset, 0.0);
    gl_Position = vec4(aPos - inc, 1.0);
    tCoord = aTexCoord;
}