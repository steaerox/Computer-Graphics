#version 110

attribute vec3 position;
attribute vec3 normal;

uniform mat4 p_matrix;

varying vec3 f_position;
varying vec3 f_normal;


void main(){
	gl_Position =  p_matrix * vec4(position, 1.0);
	f_position = position;
	f_normal = normal;
}
