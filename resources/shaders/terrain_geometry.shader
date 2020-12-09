#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 tCoord[];

out vec2 TexCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec3 point1 = gl_in[0].gl_Position.xyz;
    vec3 point2 = gl_in[1].gl_Position.xyz;
    vec3 point3 = gl_in[2].gl_Position.xyz;

    gl_Position = projection * view * model * gl_in[0].gl_Position;
    TexCoord = tCoord[0];
    vec3 norm = normalize(cross(point2 - point1, point3 - point1));
    vec4 norm4 = projection * view * model * vec4(norm, 1);
    Normal = -norm;
    EmitVertex();

    gl_Position = projection * view * model * gl_in[1].gl_Position;
    TexCoord = tCoord[1];
    norm = normalize(cross(point3 - point2, point1 - point2));
    norm4 = projection * view * model * vec4(norm, 1);
    Normal = -norm;
    EmitVertex();

    gl_Position = projection * view * model * gl_in[2].gl_Position;
    TexCoord = tCoord[2];
    norm = normalize(cross(point1 - point3, point2 - point3));
    norm4 = projection * view * model * vec4(norm, 1);
    Normal = -norm;
    EmitVertex();

    EndPrimitive();
}