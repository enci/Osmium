#version 430 core

#define kSegments 6
#define kMaxVertices 18 // 6*3
#define kPi 3.1415926

layout(points) in;
layout(triangle_strip, max_vertices = kMaxVertices) out;

uniform mat4 u_modelViewProjection;

in vec3 v_color[];
in float v_size[];

out vec3 o_color;

void main()
{
  vec4 center = u_modelViewProjection * gl_in[0].gl_Position;
  //vec4 center = gl_in[0].gl_Position;
  float radius = v_size[0];

  vec4 positions[kSegments];
  float dt = 2 * kPi / float(kSegments);
  for(int i = 0; i < kSegments; i++)
  {
    vec4 offset = vec4( sin(dt * i) * radius,
                        0.0,
                        cos(dt * i) * radius,
                        //0.0,
                        0.0);
    positions[i] = gl_in[0].gl_Position + offset;
    positions[i] = u_modelViewProjection * positions[i];
    //positions[i] = center + offset;
  }

  for(int i = 0; i < kSegments; i++)
  {
    gl_Position = center;
    o_color = v_color[0];
    EmitVertex();

    gl_Position = positions[i];
    o_color = v_color[0];
    EmitVertex();

    gl_Position = positions[(i+1) % kSegments];
    o_color = v_color[0];
    EmitVertex();

    EndPrimitive();
  }
}
