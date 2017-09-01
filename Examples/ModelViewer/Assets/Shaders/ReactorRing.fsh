#version 430 core

in vec3 v_color;
in vec2 v_texture;
in vec3 v_position;

uniform sampler2D u_texture;

out vec4 fragColor;

void main()
{
  if(abs(v_position.y) > 0.3)
    fragColor = vec4(v_color.r, v_color.g, v_color.b, 1.0);
  else
    fragColor = vec4(1.0);
}
