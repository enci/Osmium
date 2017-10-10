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
  vec2 tex = gl_FragCoord.xy / vec2(1600.0, 900.0);
  tex = tex - vec2(1);

  vec3 position = texture2D(gbf_position, tex).xyz;
  vec3 normal = texture2D(gbf_normal, tex).xyz;
  vec4 albedo = texture2D(gbf_albedo, tex);

  vec3 l = u_position - position;
  float d = length(l);
  l = normalize(l);
  float intensity = max(0.0, dot(normal, l));

  float attenuation = 1.0 - clamp((d * d) / (u_radius * u_radius), 0.0, 1.0);
  vec3 color = (u_color * albedo.rgb) * (intensity * attenuation);

  float v = d / u_radius;
  frag_color = vec4(v, v, v, 1.0);

  frag_color = vec4(color, 1.0);
}
