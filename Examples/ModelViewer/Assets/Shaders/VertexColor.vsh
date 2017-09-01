#version 430 core

// Uniforms
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_modelViewProjection;


in vec3 a_position;
in vec3 a_normal;

out vec3 v_color;

void main()
{
	v_color = a_normal;
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
