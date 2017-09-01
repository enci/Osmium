#version 430 core

#pragma include "Osmium.glsl"
#pragma include "NoiseTools.glsl"

in vec3 a_position;
in vec2 a_texture;

out vec3 v_position;
out vec2 v_texture;

void main()
{
	v_texture = a_texture;
	v_position = (u_model * vec4(a_position, 1.0)).xyz;
	gl_Position = u_projection * u_view * vec4(v_position, 1.0);
}
