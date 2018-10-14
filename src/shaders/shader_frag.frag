#version 330 core

uniform bool b_lighting;
uniform vec4 color;

in vec3 frag_normal;
out vec4 frag_color;

void main() {
	if (b_lighting) {
		frag_color = color*normalize(frag_normal).z;
    }
    else {
		frag_color = color;
    }
}
