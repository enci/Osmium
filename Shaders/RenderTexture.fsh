#version 330 core
out vec4 fragColor;

in vec2 v_texture;
uniform sampler2D s_texture;


void main()
{
  vec3 result = texture(s_texture, v_texture).rgb;

  /*
  float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
  if(brightness > 1.0)
    BrightColor = vec4(result, 1.0);
  else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

  // FragColor = vec4(result, 1.0);
  fragColor = vec4(result, 1.0);
  */

  float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
  fragColor = vec4(brightness, brightness, brightness, 1.0);
}
