#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texture;

out vec2 v_texture;

void main(void)
{
  gl_Position = vec4(a_position, 1.0);
  v_texture = a_texture;
}
