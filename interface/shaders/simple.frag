#version 330 core

uniform bool wireframe;
uniform vec4 color;

in vec3 frag_normal;
out vec4 frag_color;

void main() {
	if (wireframe) {
		// when displaying the edges of the
		// model, use a single color
		frag_color = color;
    }
    else {
		// when displaying the whole triangle,
		// use the z component of the normal
		// for basic lighting
		frag_color = color*normalize(frag_normal).z;
    }
}