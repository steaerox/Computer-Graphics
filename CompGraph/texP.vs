#version 110

attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

uniform mat4 p_matrix;

varying vec3 f_position;
varying vec3 f_normal;
varying vec2 f_uv;


void main(){
	gl_Position =  p_matrix * vec4(position, 1.0);
	f_position = position;
	f_normal = normal;
	f_uv = uv;
}
