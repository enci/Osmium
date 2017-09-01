#version 430 core

const float kLightRadius = 4.2;

// Uniforms
uniform mat4 u_modelViewProjection;

// Vertex atributes
in vec3 a_position;
in float a_size;
in vec4 a_color;
in vec3 a_axis;
in float a_rotation;

// Vertex shader outputs
out vec4 v_color;
out float v_size;
out vec3 v_axis;
out float v_rotation;

void main()
{
  v_color = a_color;
  v_size = a_size;
  v_axis = a_axis;
  v_rotation = a_rotation;
  gl_Position = vec4(a_position, 1.0);
}
