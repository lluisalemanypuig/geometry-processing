#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertex_colour;
layout (location = 2) in vec3 normal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat3 normal_matrix;

out vec3 vert_col;

void main() {
	gl_Position = projection*modelview*vec4(position, 1.0);
	vert_col = vertex_colour;
}

