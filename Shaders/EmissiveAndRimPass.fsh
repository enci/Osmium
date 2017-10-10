#version 330 core

uniform sampler2D gbf_position;
uniform sampler2D gbf_normal;
uniform sampler2D gbf_albedo;
uniform vec3 u_eyePos;
const float kRimGamma = 4.0;

in vec2 v_texture;

out vec4 frag_color;


vec3 CalculateRimLight(in vec3 normal, in vec3 albedo)
{
  float rim = 1.0 - max(0.0, dot(normal, normalize(u_eyePos)));
  rim = pow(rim, kRimGamma) * 1.0;
  return albedo * rim;
}

void main( void )
{
    vec3 position = texture2D(gbf_position, v_texture).xyz;
    vec3 normal = texture2D(gbf_normal, v_texture).xyz;
    vec4 albedo = texture2D(gbf_albedo, v_texture);

    vec3 color = albedo.a * albedo.rgb;
    color += CalculateRimLight(normal, albedo.rgb);
    frag_color = vec4(color, 1.0);

    // Color
    // frag_color = vec4(color, 1.0);

    // Albedo
    // frag_color = vec4(albedo.rgb, 1.0);

    // Emissive
    // frag_color = vec4(albedo.a, albedo.a, albedo.a, 1.0);

    // Position
    // frag_color = vec4(position, 1.0);

    // Normal
    // frag_color = vec4(normal, 1.0);
}
