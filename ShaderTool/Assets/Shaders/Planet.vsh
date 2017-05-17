#version 430 core

#pragma include "Osmium.glsl"
#pragma include "NoiseTools.glsl"

#define USE_RIM 0

const float kRimGamma = 4.0;
const float kLightRadius = 5.2;

// Vertex atributes
in vec3 a_position;
in vec3 a_normal;

// Vertex shader outputs
out vec3 vg_normal;
out vec3 vg_position;

void main()
{
	vec3 normal = normalize((u_model * vec4(a_normal, 0.0)).xyz);
	vec3 worldPosition = a_position;
	float offset = sample_noise(worldPosition);
	worldPosition += normal * offset;
	worldPosition = (u_model * vec4(worldPosition, 1.0)).xyz;

	gl_Position = u_projection * u_view * vec4(worldPosition, 1.0);

  vg_normal = normal;
  vg_position = worldPosition;
}
