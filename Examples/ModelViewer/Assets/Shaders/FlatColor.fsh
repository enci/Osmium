#version 430 core

uniform vec3 u_diffuse;

out vec4 fragColor;

void main()
{
  fragColor = vec4(u_diffuse.r, u_diffuse.g, u_diffuse.b, 1.0);
}
