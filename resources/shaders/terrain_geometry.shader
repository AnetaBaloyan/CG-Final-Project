#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 tCoord[];
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

    gl_Position = projection * view * model * gl_in[0].gl_Position;
    TexCoord = tCoord[0];
    EmitVertex();

    gl_Position = projection * view * model * gl_in[1].gl_Position;
    TexCoord = tCoord[1];
    EmitVertex();

    gl_Position = projection * view * model * gl_in[2].gl_Position;
    TexCoord = tCoord[2];
    EmitVertex();

    EndPrimitive();
}