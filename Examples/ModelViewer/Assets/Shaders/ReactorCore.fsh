#version 430 core

// #pragma include "Osmium.glsl"
//#pragma include "NoiseTools.glsl"

// const float eps = 0.01;
// const int octaves = 1;

// in vec3 v_normal;
// in vec3 v_position;
in vec3 v_color;

out vec4 fragColor;

void main()
{
  fragColor = vec4(v_color.r, v_color.g, v_color.b, 1.0);
  /*
  vec3 color = u_ambient;
  vec3 spos = v_position;
  float f = sample_noise(spos);
  float fx = sample_noise(spos + vec3(eps, 0.0, 0.0));
  float fy = sample_noise(spos + vec3(0.0, eps, 0.0));
  float fz = sample_noise(spos + vec3(0.0, 0.0, eps));

  vec3 normal = normalize(v_normal);
  vec3 dn = vec3(fx - f, fy - f, fz - f) * 1 / eps * 0.7;
  //normal = normalize(normal - dn);

  // Caluate color based on light / normal
  color += CalculateDirectionalLightsSpecular(normal, v_position, 40.0);
  //color += CalculateDirectionalLights(normal);

  // Just output raw noise value
  // color = vec3(f);

  // Wood
  f *= 10.0;
  f = f - floor(f);
  //color *= texture(u_texture, vec2(f, 0.5)).rgb;

  //color = u_directionalLights[0].direction;
  // color = v_normal;

  // vec3 color = v_color;
  // vec3 color =  vec3((noise + 1.0) * 0.5);
  fragColor = vec4(color.r, color.g, color.b, 1.0);
  */
}
