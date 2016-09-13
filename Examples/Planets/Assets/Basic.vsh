#version 430 core

// Uniforms
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

// Vertex atributes
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texture;

// Hardcoded light - assuming completely white light
vec3 u_light_direction = vec3(1.0, 2.0, 3.0);

// Vertex shader outputs
out vec2 v_texture;
out float v_intensity;

void main()
{
	vec3 normal = normalize((u_model * vec4(a_normal, 0.0)).xyz);
	vec3 light_dir = normalize(u_light_direction);
	v_intensity = max(0.0, dot(normal, light_dir));
	v_texture = a_texture;
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
