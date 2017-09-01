#version 430 core

#pragma include "Osmium.glsl"
#pragma include "NoiseTools.glsl"

// const float eps = 0.01;
// const int octaves = 1;

// in vec3 v_normal;
in vec3 v_position;
in vec2 v_texture;

out vec4 fragColor;

void main()
{
  vec3 spos = v_position;
  //float ns = 1.0 - 0.09 * turbulence(spos, 0.06);                       // Riges turbulence(spos * 0.1, 1.0);
  float ns = snoise(vec3(v_texture.x, v_texture.y, u_time * 0.05) * 30.0);        // Riges turbulence(spos * 0.1, 1.0);
  //ns *= 100.0f;
  if(ns < 0.75)
    discard;
  fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
