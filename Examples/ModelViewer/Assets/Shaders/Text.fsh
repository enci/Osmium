#version 430 core

in vec3 v_color;
in vec2 v_texture;

uniform sampler2D u_texture;
uniform vec3 u_diffuse;

out vec4 fragColor;

void main()
{
  float alpha = texture(u_texture, v_texture).r;
  if(alpha < 0.2f)
    discard;
  // if(alpha < 0.99f)
  //  fragColor = vec4(0.0, 0.0, 0.0, 1.0);
  else
    fragColor = vec4(u_diffuse, 1.0); // alpha * vec4(v_color.r, v_color.g, v_color.b, 1.0);
}
