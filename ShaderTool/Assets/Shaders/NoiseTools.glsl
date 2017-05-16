#pragma include "Noise.glsl"

const float W = 400.0;
const float PI = 3.14;
const float Speed = 0.1;
const float Frequency = 10.5;

// Sample noise with frequncy. Hide algorithm
float noise(vec3 p, float f)
{
  // Animate noise
  // vec4 pos = vec4(p.x, p.y, p.z, u_time * Speed);
  // return cnoise(pos * f);

  // Classic noise
  // return cnoise(p * f);

  // Simplex noise
  //return snoise(p * f);

  // Value noise
  return vnoise(p * f);
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

// This is the one the game is using
float sample_noise(vec3 pos)
{
  // Put to [0,1] range
  //return (noise(pos, Frequency) + 1.0) * 0.5;

  // Return plain
  return noise(pos, Frequency);

  //return 0.01 * stripes(pos.x + 2 * turbulence(pos, 1.0), 1.6);  // Marbled
  //return noise(pos, 8.0) * 0.04;                                // Simple
  //return -0.10 * turbulence(pos, 1);                       // Riges
  //return 0.0;
}
