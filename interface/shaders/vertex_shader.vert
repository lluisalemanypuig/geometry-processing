#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 vertex_colour;
layout (location = 3) in vec2 texture_coords;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 modelview_inverse;
uniform mat3 normal_matrix;

out vec3 frag_normal;
out vec3 frag_pos_world;
out vec3 vert_col;
out vec2 tex_coord;

void main() {
	gl_Position = projection*modelview*vec4(position, 1.0);
	frag_normal = normal_matrix*normal;
	frag_pos_world = vec3(modelview_inverse*vec4(position, 1.0));
	vert_col = vertex_colour;
	tex_coord = texture_coords;
}

