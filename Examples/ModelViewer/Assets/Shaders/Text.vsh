#version 430 core

// Uniforms
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_modelViewProjection;
uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_eyePos;
uniform float u_fogFar;
uniform float u_fogNear;
uniform float u_fogExp;
uniform vec4 u_fogColorNear;
uniform vec4 u_fogColorFar;

// Vertex atributes
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texture;

// Vertex shader outputs
out vec2 v_texture;
out vec3 v_color;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
	v_color = u_diffuse;
	v_texture = a_texture;
}
