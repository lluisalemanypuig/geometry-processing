#version 330 core

uniform bool b_lighting;
uniform vec4 color;

in vec3 vert_col;
out vec4 frag_color;

void main() {
	if (b_lighting) {
		frag_color = vec4(vert_col, 1);
	}
	else {
		frag_color = color;
	}
}
