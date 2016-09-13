#version 430 core

in float v_intensity;
in vec2 v_texture;

uniform sampler2D u_texture;

out vec4 fragColor;

void main()
{
  fragColor = texture(u_texture, v_texture) *
              vec4(v_intensity, v_intensity, v_intensity, 1.0);
}
