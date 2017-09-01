#version 430 core

#pragma include "Osmium.glsl"

// Vertex atributes
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texture;

// Vertex shader outputs
out vec2 v_texture;
out vec3 v_color;
out vec3 v_normal;
out vec3 v_position;
out vec4 v_fog;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

	vec3 normal = normalize((u_model * vec4(a_normal, 0.0)).xyz);
	vec3 worldPosition = (u_model * vec4(a_position, 1.0)).xyz;
	v_color = u_ambient;

	vec3 vPos = (u_view * u_model * vec4(a_position, 1.0)).xyz;
  vec3 toEye = normalize(-vPos);
	vec3 vNormal =  normalize((u_view * u_model * vec4(a_normal, 0.0)).xyz);
	float rim = 1 - max(0.0, dot(vNormal, toEye));
  rim = pow(rim, kRimGamma);
  v_color += vec3(u_diffuse * rim);

  v_color += CalculateDirectionalLights(normal);

 	// Point lights
	for(int i = 0; i < u_pointLightsCount && i < POINT_LIGHT_COUNT; i++)
	{
		vec3 light_dir = u_pointLights[i].position - worldPosition;
		float dist = length(light_dir);
		light_dir = normalize(light_dir);
		float intensity = max(0.0, dot(normal, light_dir));
		float attenuation = 1.5 / (1.0 + (dist / kLightRadius));
		v_color += u_pointLights[i].color * intensity * attenuation * u_diffuse;
	}

	float param = (u_fogFar - length(vPos)) / (u_fogFar - u_fogNear);
	param = clamp(1 - param, 0.0, 1.0);
	v_fog = mix(u_fogColorNear, u_fogColorFar, param);
  v_fog.a *= param;
	//v_color = mix(v_color, fogColor.rgb, param * fogColor.a);
	v_texture = a_texture;

  v_normal = normal;
  v_position = worldPosition;
}
