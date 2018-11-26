#version 330 core

uniform bool curvature;
uniform bool wireframe;
uniform bool reflection_lines;
uniform vec4 color;

in vec3 vert_col;
in vec3 frag_normal;
out vec4 frag_color;

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
		vec3 v = vec3(0,0,-100);
		vec3 a = vec3(100,0,0);

		float lv = length(v);
		vec3 r = (2/(lv*lv))*(dot(frag_normal, v)*frag_normal - v);
		vec3 d = r - dot(r,a)*a;
		vec3 va_star = v - dot(v,a)*a;
		vec3 va = normalize(va_star);
		vec3 a_perp = cross(a, va);

		float theta = atan(dot(r,a_perp), dot(r, va));

		if (theta < 0) {
			frag_color = vec4(vec3(0), 1);
		}
		else {
			frag_color = vec4(1);
		}

		//float theta_norm = theta/(2*3.141592);
		//frag_color = vec4(vec3(theta_norm), 1);
		//frag_color = frag_color*normalize(frag_normal).z;
	}
    else {
		// when displaying the whole triangle,
		// use the z component of the normal
		// for basic lighting
		frag_color = color*normalize(frag_normal).z;
	}
}
