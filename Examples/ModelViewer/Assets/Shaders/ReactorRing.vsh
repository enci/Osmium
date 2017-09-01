#version 430 core

#pragma include "Osmium.glsl"

// Vertex atributes
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texture;

// Vertex shader outputs
out vec2 v_texture;
out vec3 v_color;
out vec3 v_position;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

	vec3 normal = normalize((u_model * vec4(a_normal, 0.0)).xyz);
	vec3 worldPosition = (u_model * vec4(a_position, 1.0)).xyz;
	v_color = u_ambient;

  v_color += CalculateRimLight(worldPosition, a_normal);

  v_color += CalculateDirectionalLights(normal);

  v_color += CalculatePointLights(worldPosition, normal);

	//float param = 1; (u_fogFar - length(-worldPosition.y)) / (u_fogFar - u_fogNear);
	//param = clamp(1 - param, 0.0, 1.0);
	//vec4 fogColor = mix(u_fogColorNear, u_fogColorFar, param);
	//v_color = mix(v_color, fogColor.rgb, param * fogColor.a);
	v_texture = a_texture;
  v_position = worldPosition;
}
