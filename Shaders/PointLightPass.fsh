#version 330 core

uniform vec3 u_position;
uniform vec3 u_color;
uniform float u_radius;

in vec2 v_texture;

uniform sampler2D gbf_position;
uniform sampler2D gbf_normal;
uniform sampler2D gbf_albedo;

out vec4 frag_color;

void main(void)
{
  vec3 position = texture2D(gbf_position, v_texture).xyz;
  vec3 normal = texture2D(gbf_normal, v_texture).xyz;
  vec4 albedo = texture2D(gbf_albedo, v_texture);

  vec3 l = u_position - position;
  float d = length(l);
  l = normalize(l);
  float intensity = max(0.0, dot(normal, l));

  float attenuation = 1.0 - clamp((d * d) / (u_radius * u_radius), 0.0, 1.0);
  vec3 color = (u_color * albedo.rgb) * (intensity * attenuation);

  frag_color = vec4(color, 1.0);
}
