#version 430 core

#pragma include "Osmium.glsl"
#pragma include "Noise.glsl"

const float PI = 3.14;
const float eps = 0.01;
const int octaves = 1;
const float W = 400.0;

in vec3 v_color;
in vec2 v_texture;
in vec3 v_normal;
in vec3 v_position;
in vec4 v_fog;

uniform sampler2D u_texture;

out vec4 fragColor;
/*
float n = 0.0;
float freq = 1.0;
for(int i = 0; i < octaves; i++)
{
  freq = pow(2, i);
  n += cnoise(p * freq) * (1 / freq);
}
return n;
*/

// Sample noise with frequncy. Hide algorithm
float noise(vec3 p, float f)
{
  return cnoise(p * f);
}

float turbulence(vec3 pos, float f)
{
  float t = -0.5;
  for (; f <= W / 12; f *= 2.0) // W = Image width in pixels
    t += abs(noise(pos, f) / f);
  return t;
}

float stripes(float x, float f)
{
  float t = 0.5 + 0.5 * sin(f * 2 * PI * x);
  return t * t - 0.5;
}

float sample_noise(vec3 pos)
{
   return 0.01 * stripes(pos.x + 2 * turbulence(pos, 1.0), 1.6);  //MARBLED
  // return noise(pos, 8.0) * 0.04;
  // return -0.10 * turbulence(pos, 1);                       //CRINKLED

  //return 0.0;
}

void main()
{
  vec3 color = u_ambient;
  vec3 spos = v_position;
  float f = sample_noise(spos);
  float fx = sample_noise(spos + vec3(eps, 0.0, 0.0));
  float fy = sample_noise(spos + vec3(0.0, eps, 0.0));
  float fz = sample_noise(spos + vec3(0.0, 0.0, eps));

  vec3 normal = normalize(v_normal);
  vec3 dn = vec3(fx - f, fy - f, fz - f) * 1 / eps;
  normal = normalize(normal - dn);

  color += CalculateDirectionalLightsSpecular(normal, v_position, 40.0);
  //color = u_directionalLights[0].direction;
  //color = v_normal;


  // vec3 color = v_color;
  // vec3 color =  vec3((noise + 1.0) * 0.5);
  fragColor = vec4(color.r, color.g, color.b, 1.0);
}
