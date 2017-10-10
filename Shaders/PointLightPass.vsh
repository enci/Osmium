#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texture;

uniform mat4 u_modelViewProjection;

out vec2 v_texture;

void main(void)
{
  gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
  v_texture = gl_Position.xy / gl_Position.z;
}
