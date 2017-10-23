#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location = 0)   in      vec3 a_position;
layout (location = 0)   uniform mat4 u_modelViewProjection;

void main()
{
    gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}