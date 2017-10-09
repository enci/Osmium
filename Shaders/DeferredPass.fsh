#version 330 core

#pragma include "Osmium.glsl"

vec3 CalculateDirectionalLights(in vec3 normal, in vec3 albedo)
{
  vec3 color = vec3(0.0, 0.0, 0.0);

  for(int i = 0; i < u_directionalLightsCount && i < DIR_LIGHT_COUNT; i++)
  {
    vec3 light_dir = u_directionalLights[i].direction;
    float intensity = max(0.0, dot(normal, light_dir));
    color += u_directionalLights[i].color * intensity * albedo;
  }

  return color;
}

vec3 CalculatePointLights(
  in vec3 position,
  in vec3 normal,
  in vec3 albedo)
{
  vec3 color = vec3(0.0, 0.0, 0.0);

  for(int i = 0; i < u_pointLightsCount && i < POINT_LIGHT_COUNT; i++)
  {
    vec3 light_dir = u_pointLights[i].position - position;
    float dist = length(light_dir);
    light_dir = normalize(light_dir);
    float intensity = max(0.0, dot(normal, light_dir));

    vec3 l = u_pointLights[i].position - position;
    float d = length(l);
    float attenuation = 1.0 - clamp((d * d) / (u_pointLights[i].radius * u_pointLights[i].radius), 0.0, 1.0);
    color += u_pointLights[i].color * intensity * attenuation * albedo;
  }

  return color;
}

vec3 CalculateRimLight(in vec3 normal, in vec3 albedo)
{
  float rim = 1.0 - max(0.0, dot(normal, normalize(u_eyePos)));
  rim = pow(rim, kRimGamma) * 1.0;
  return albedo * rim;
}

uniform sampler2D gbf_position;
uniform sampler2D gbf_normal;
uniform sampler2D gbf_albedo;

in vec2 v_texture;
out vec4 fragColor;

void main( void )
{
    vec3 position = texture2D(gbf_position, v_texture).xyz;
    vec3 normal = texture2D(gbf_normal, v_texture).xyz;
    vec4 albedo = texture2D(gbf_albedo, v_texture);

    vec3 color = albedo.a * albedo.rgb;
    color += CalculateDirectionalLights(normal, albedo.rgb);
    color += CalculatePointLights(position, normal, albedo.rgb);
    color += CalculateRimLight(normal, albedo.rgb);

    // Color
    // fragColor = vec4(color, 1.0);

    // Albedo
    //fragColor = vec4(albedo.rgb, 1.0);

    // Emissive
    // fragColor = vec4(albedo.a, albedo.a, albedo.a, 1.0);

    // Position
    // fragColor = vec4(position, 1.0);

    // Normal
    fragColor = vec4(normal, 1.0);
}
