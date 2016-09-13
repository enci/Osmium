#version 430 core


in vec4 v_color;
in vec2 v_texture;

uniform sampler2D u_texture;

out vec4 fragColor;

void main()
{

	float dist = gl_FragCoord.z / gl_FragCoord.w;

  fragColor = texture(u_texture, v_texture) * dist
              vec4(v_color.r, v_color.g, v_color.b, 1.0);
}
