#version 430 core

in vec3 o_color;

uniform sampler2D u_texture;

out vec4 fragColor;

void main()
{
  fragColor = vec4(o_color, 1.0);
}
