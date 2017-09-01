#version 430 core

//uniform vec3 u_diffuse;

out vec4 fragColor;

in vec3 v_color;

void main()
{
  fragColor = vec4(v_color.r, v_color.g, v_color.b, 1.0);
}
