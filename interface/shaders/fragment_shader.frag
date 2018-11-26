#version 330 core

uniform bool curvature;
uniform bool wireframe;
uniform bool reflection_lines;
uniform vec4 color;

in vec3 vert_col;
in vec3 frag_normal;
out vec4 frag_color;

vec4 reflection_line(vec3 a, vec3 v) {
	vec3 n = frag_normal;

	float lv = length(v);
	vec3 r = (2/(lv*lv))*(dot(n, v)*n - v);
	vec3 d = r - dot(r,a)*a;
	vec3 av_star = v - dot(v,a)*a;
	vec3 av = normalize(av_star);
	vec3 a_perp = cross(a, av);

	float theta = atan(dot(r,a_perp), dot(r, av));
	if (theta < 0) {
		return vec4(vec3(0), 1);
	}
	else {
		return vec4(1);
	}
}

void main() {
	if (wireframe && curvature || curvature) {
		frag_color = vec4(vert_col, 1);
	}
	else if (wireframe) {
		// when displaying the edges of the
		// model, use a single color
		frag_color = color;
	}
	else if (reflection_lines) {
		vec3 v = vec3(0,0,-1);
		vec3 a = vec3(1,0,0);

		frag_color = reflection_line(a, -vec3(gl_FragCoord));
	}
    else {
		// when displaying the whole triangle,
		// use the z component of the normal
		// for basic lighting
		frag_color = color*normalize(frag_normal).z;
	}
}
