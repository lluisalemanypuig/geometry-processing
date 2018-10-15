#version 330 core

uniform bool wireframe;
uniform vec4 color;

in vec3 vert_col;
out vec4 frag_color;

void main() {
	if (wireframe) {
		// when displaying the edges of the
		// model, use a single color
		frag_color = color;
	}
	else {
		// when displaying the whole triangle,
		// use the color assigned to the vertex
		frag_color = vec4(vert_col, 1);
	}
}
