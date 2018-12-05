#version 330 core

uniform bool curvature;
uniform bool wireframe;

uniform bool reflection_lines;
uniform int num_sources;
uniform vec3 viewer_pos;

uniform bool harmonic_maps;
uniform bool remeshing;

uniform vec4 color;

in vec3 frag_normal;	// normal of the fragment
in vec4 vert_pos_world;	// position of the vertex in world coordinates
in vec3 vert_col;		// colour of the vertex
in vec2 tex_coord;		// texture coordinates [-1,-1]x[1,1]
out vec4 frag_color;	// color of the fragment

// Reflection Lines
vec4 RL(vec3 a, vec3 v) {
	vec3 n = normalize(
		cross(
			vec3(dFdx(vert_pos_world)),
			vec3(dFdy(vert_pos_world))
		)
	);

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
	// grid of 10x10
	const int N = 10;

	// in our application, texture coordinates
	// are in the space [-1,-1]x[1,1]
	float s = tex_coord.x + 1;
	float t = tex_coord.y + 1;

	int fs = int((N*N)*s);
	int ft = int((N*N)*t);

	if (mod(fs, N) == 0 || mod(ft, N) == 0) {
		frag_color = vec4(0.0,0.0,0.0,0.0);
	}
	else {
		discard;
	}
}

// Harmonic Maps Checkered
// render a checkered board using the
// texture coordinates
void HM_checkered() {
	// grid of 10x10
	const int N = 10;

	// in our application, texture coordinates
	// are in the space [-1,-1]x[1,1]
	float s = tex_coord.x + 1;
	float t = tex_coord.y + 1;

	/*
	// An alternative to the code being used is:
	vec4 col;
	int ks = int(N*s); // 1 <= ks <= 10
	int kt = int(N*t); // 1 <= kt <= 10
	if (ks%2 == 0) {
		if (kt%2 == 0) col = vec4(0.0, 0.0, 0.0, 1.0);
		else col = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else {
		if (kt%2 == 0) col = vec4(1.0, 1.0, 1.0, 1.0);
		else col = vec4(0.0, 0.0, 0.0, 1.0);
	}
	frag_color = col;
	*/
	/*
	// There is yet another simplification
	// of the same code
	vec4 col;
	int ks = int(N*s); // 1 <= ks <= 10
	int kt = int(N*t); // 1 <= kt <= 10
	if (ks%2 == kt%2) {
		col = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {
		col = vec4(1.0, 1.0, 1.0, 1.0);
	}
	frag_color = col;
	*/

	// when 'ks' has the same parity as 'kt' we have
	// that the parity of the sum 'ks + kt' is even
	int sum = int(N*s) + int(N*t);
	frag_color = vec4(1.0,1.0,1.0,1.0);
	if (mod(sum, 2) == 0) {
		frag_color = vec4(0.0,0.0,0.0,1.0);
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
		vec3 v = viewer_pos - vec3(vert_pos_world);
		vec3 a = vec3(0,0,1);
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
