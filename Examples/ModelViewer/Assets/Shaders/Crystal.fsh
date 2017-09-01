#version 430 core

#pragma include "Osmium.glsl"

in vec3 v_color;
in vec2 v_texture;
in vec3 v_normal;
in vec3 v_position;
in vec4 v_fog;

uniform sampler2D u_texture;

out vec4 fragColor;

void main()
{
  vec3 toEye = normalize(u_eyePos - v_position);
  vec3 ref = reflect(toEye, normalize(v_normal));
  float d =  dot(ref, normalize(vec3(0.5, 0.0, 1.0)));
  d = 1.0 - abs(d);
  d = pow(d, 32);
  d *= 1.2;

  vec3 color = v_color + vec3(d);
  //color = mix(color, v_fog.rgb, v_fog.a);

  fragColor = texture(u_texture, v_texture).a *
              vec4(color.r, color.g, color.b, 1.0);
}
