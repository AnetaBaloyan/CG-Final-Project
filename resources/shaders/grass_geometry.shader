#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 tCoord[];
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D grassDist;

void generateGrassObj(vec4 position, float size) {

	vec4 bottom_left = position + vec4(-size, 0, 0, 0);
	gl_Position = projection * view * model * bottom_left;
	TexCoord = vec2(0, 0);
	EmitVertex();

	vec4 top_left = position + vec4(-size, size*2, 0, 0);
	gl_Position = projection * view * model * top_left;
	TexCoord = vec2(0, 1);
	EmitVertex();

	vec4 bottom_right = position + vec4(size, 0, 0, 0);
	gl_Position = projection * view * model * bottom_right;
	TexCoord = vec2(1, 0);
	EmitVertex();

	vec4 top_right = position + vec4(size, size * 2, 0, 0);
	gl_Position = projection * view * model * top_right;
	TexCoord = vec2(1, 1);
	EmitVertex();

	EndPrimitive();
}

void main() {
	vec4 A = gl_in[0].gl_Position;
	vec4 B = gl_in[1].gl_Position;
	vec4 C = gl_in[2].gl_Position;
	vec4 center = (A + B + C) / 3;
	vec2 center_tex = (tCoord[0] + tCoord[1] + tCoord[2]) / 3;

	vec4 texColor = texture(grassDist, center_tex);

	if (texColor.x > 0.9 && texColor.y > 0.9 && texColor.z > 0.9) {
		generateGrassObj(center, 0.1);
	}
}