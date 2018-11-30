#version 330 core

uniform bool curvature;
uniform bool wireframe;

uniform bool reflection_lines;
uniform int num_sources;
uniform vec3 viewer_pos;

uniform bool harmonic_maps;
uniform bool remeshing;

uniform vec4 color;

in vec3 frag_normal;
in vec3 frag_pos_world;
in vec3 vert_col;
in vec2 tex_coord;
out vec4 frag_color;

// Reflection Lines
vec4 RL(vec3 a, vec3 v) {
	vec3 n = frag_normal;

	float lv = length(v);
	vec3 r = (2/(lv*lv))*(dot(n, v)*n - v);
	vec3 d = r - dot(r,a)*a;
	vec3 av_star = v - dot(v,a)*a;
	vec3 av = normalize(av_star);
	vec3 a_perp = cross(a, av);

	float theta = atan(dot(r,a_perp), dot(r, av));
	theta /= (2*3.141592);
	theta *= (2*num_sources);
	if (int(theta)%2 == 0) {
		return vec4(vec3(0), 1);
	}
	else {
		return vec4(1);
	}
}

// Harmonic Maps Remeshing
// do not render 'interior' fragments
void HM_remeshing() {

}

// Harmonic Maps Checkered
// render a checkered board using the
// texture coordinates
void HM_checkered() {
	float total =	floor(tex_coord.x*10) +
					floor(tex_coord.y*10);

	bool is_even = mod(total, 2.0) == 0.0;
	if (is_even) {
		// black
		frag_color = vec4(0.0,0.0,0.0,1.0);
	}
	else {
		// white
		frag_color = vec4(1.0,1.0,1.0,1.0);
	}
}

void main() {
	if (curvature && wireframe || curvature) {
		frag_color = vec4(vert_col, 1);
	}
	else if (wireframe) {
		// when displaying the edges of the
		// model, use a single color
		frag_color = color;
	}
	else if (reflection_lines) {
		vec3 v = viewer_pos - frag_pos_world;
		vec3 a = vec3(1,0,0);
		frag_color = RL(a, v);
	}
	else if (harmonic_maps) {
		if (remeshing) {
			// do not render 'interior' fragments
			HM_remeshing();
		}
		else {
			// render a regular square grid
			// black and white
			HM_checkered();
		}
	}
	else {
		// when displaying the whole triangle,
		// use the z component of the normal
		// for basic lighting
		frag_color = color*normalize(frag_normal).z;
	}
}
