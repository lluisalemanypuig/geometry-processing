#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 vertex_colour;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat3 normal_matrix;

out vec3 vert_col;
out vec3 frag_normal;

void main() {
	gl_Position = projection*modelview*vec4(position, 1.0);
	frag_normal = normal_matrix*normal;
	vert_col = vertex_colour;
}

