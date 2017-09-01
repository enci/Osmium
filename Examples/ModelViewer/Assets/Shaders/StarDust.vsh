#version 430 core

const float kLightRadius = 4.2;

// Uniforms
uniform mat4 u_modelViewProjection;

// Vertex atributes
in vec3 a_position;
in float a_size;
in vec3 a_color;

// Vertex shader outputs
//out vec2 v_texture;
out vec3 v_color;
out float v_size;

void main()
{
  v_color = a_color;
  v_size = a_size;
  gl_Position =  vec4(a_position, 1.0);
}
