#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 tCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D texture2;

void main()
{
    vec4 filt = texture2D(texture2, aTexCoord);
    float offset = filt.r / 2;
    vec3 inc = vec3(0.0, -offset, 0.0);
    /*vec4 texColor = texture(texture2, aTexCoord);
    vec3 v = aPos;
    if (texColor.x == 0.0 && texColor.y == 0.0 && texColor.z == 0.0) {
        v = v - vec3(0.0, 0.5, 0.0);
    }*/
    gl_Position = vec4(aPos - inc, 1.0);
    tCoord = aTexCoord;
}