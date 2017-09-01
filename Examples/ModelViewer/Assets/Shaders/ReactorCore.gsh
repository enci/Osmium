#version 430 core

#pragma include "Osmium.glsl"

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 vg_normal[];
in vec3 vg_position[];

// Vertex shader outputs
// out vec3 v_normal;
// out vec3 v_position;
out vec3 v_color;

void main()
{
  // Calculate normal from triangle (set to face)
  vec3 d0 = vg_position[1] - vg_position[0];
  vec3 d1 = vg_position[2] - vg_position[0];
  vec3 normal = normalize(cross(d0, d1));

  for(int i = 0; i < 3; i++)
  {
    gl_Position = gl_in[i].gl_Position;
    // v_normal = vg_normal[i];
    // v_normal = normal;
    // v_position = vg_position[i];
    v_color = u_ambient;
    v_color += CalculateDirectionalLightsSpecular(normal, vg_position[i], 40.0);
    v_color += CalculateRimLight(vg_position[i], normal);

    EmitVertex();
  }
  EndPrimitive();
}
