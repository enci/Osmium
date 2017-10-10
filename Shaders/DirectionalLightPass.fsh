#version 330 core

uniform vec3 u_direction;
uniform vec3 u_color;

in vec2 v_texture;

uniform sampler2D gbf_normal;
uniform sampler2D gbf_albedo;

out vec4 frag_color;

void main(void)
{
  vec3 normal = texture2D(gbf_normal, v_texture).xyz;
  vec4 albedo = texture2D(gbf_albedo, v_texture);

  float intensity = max(0.0, dot(normal, u_direction));
  frag_color = vec4((u_color * albedo.xyz) * intensity, 1.0);

  //frag_color = vec4(intensity, intensity, intensity, 1.0);
}
