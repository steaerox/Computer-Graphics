#version 110

varying vec3 f_position;
varying vec3 f_normal;

uniform vec3 f_color;
uniform vec3 f_spec_color;
uniform float f_spec_power;
uniform vec3 l_dir;
uniform vec3 l_color;
uniform float l_amb;
uniform vec3 eye_pos;

void main() {
	vec3 amb_diff;
	vec3 spec;
	vec3 n_l_dir;
	vec3 n_normal = normalize(f_normal);
	vec3 eye_dir = normalize(f_position - eye_pos);
	float l_dim, l_cone;

	l_dim = 1.0;
	n_l_dir = -normalize(l_dir);

	vec3 reflection = reflect(n_l_dir, n_normal);
	
	amb_diff = f_color * l_color * (max(dot(n_normal, n_l_dir), 0.0) * l_dim + l_amb);
	spec = pow(max(dot(reflection, eye_dir), 0.0), f_spec_power) * f_spec_color * l_color * l_dim;

	gl_FragColor = vec4(min(amb_diff + spec, vec3(1,1,1)), 1);
}
