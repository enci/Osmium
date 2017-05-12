#version 430 core

#pragma include "Osmium.glsl"

#define USE_RIM 0

const float kRimGamma = 4.0;
const float kLightRadius = 5.2;

// Vertex atributes
in vec3 a_position;
in vec3 a_normal;

// Vertex shader outputs
out vec2 v_texture;
out vec3 v_color;
out vec3 v_normal;
out vec3 v_position;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

	vec3 normal = normalize((u_model * vec4(a_normal, 0.0)).xyz);
	vec3 worldPosition = (u_model * vec4(a_position, 1.0)).xyz;
	v_color = u_ambient;

	vec3 vPos = (u_view * u_model * vec4(a_position, 1.0)).xyz;
  vec3 toEye = normalize(-vPos);
	vec3 vNormal =  normalize((u_view * u_model * vec4(a_normal, 0.0)).xyz);

#if USE_RIM
	float rim = 1 - max(0.0, dot(vNormal, toEye));
  rim = pow(rim, kRimGamma);
  v_color += vec3(u_diffuse * rim);
#endif

  //v_color += CalculateDirectionalLights(normal);
	v_color += CalculateDirectionalLightsSpecular(normal, worldPosition, 4.0);

  v_normal = normal;
  v_position = worldPosition;
}
