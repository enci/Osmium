#version 430 core

#define DIR_LIGHT_COUNT     5
#define POINT_LIGHT_COUNT   10
const float sqrtTwo  = 1.41421356237;
const vec3 lightDir = vec3(1.0, 2.0, 3.0);

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
};

struct PointLight
{
    vec3 position;
    vec3 color;
};

layout(points) in;
layout(triangle_strip, max_vertices=12) out;

uniform mat4 u_modelViewProjection;
uniform int u_directionalLightsCount;
uniform int u_pointLightsCount;
uniform vec3 u_ambient;

// Lights
uniform DirectionalLight    u_directionalLights[DIR_LIGHT_COUNT];
uniform PointLight          u_pointLights[POINT_LIGHT_COUNT];

in vec4 v_color[];
in float v_size[];
in vec3 v_axis[];
in float v_rotation[];

out vec3 o_color;

mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

vec3 litColor(in vec3 normal, in vec3 tint)
{
  vec3 color = vec3(0.0);

  for(int i = 0; i < u_directionalLightsCount && i < DIR_LIGHT_COUNT; i++)
  {
    vec3 lightDir = u_directionalLights[i].direction;
    float intensity = max(0.0, dot(normal, lightDir));
    color += u_directionalLights[i].color * intensity;
  }

  return color * tint;

/*
  // Point lights
  for(int i = 0; i < u_pointLightsCount && i < POINT_LIGHT_COUNT; i++)
  {
    vec3 light_dir = u_pointLights[i].position - worldPosition;
    float dist = length(light_dir);
    light_dir = normalize(light_dir);
    float intensity = max(0.0, dot(normal, light_dir));
    float attenuation = 1.5 / (1.0 + (dist / kLightRadius));
    v_color += u_pointLights[i].color * intensity * attenuation;
  }
  */
}

void main()
{
  vec3 color = v_color[0].rgb;
  vec3 emmisive = color * v_color[0].a;
  vec4 center = gl_in[0].gl_Position;
  float size = v_size[0] * 0.5;
  mat3 trns = rotationMatrix(v_axis[0], v_rotation[0]);

  vec4 A = vec4(1, -1, -1, 0) * size;
  vec4 B = vec4(1, 1, 1, 0) * size;
  vec4 C = vec4(-1, 1, -1, 0) * size;
  vec4 D = vec4(-1, -1, 1, 0) * size;
  vec3 ACD = vec3(-1, -1, -1);
  vec3 ABD = vec3(1, -1, 1);
  vec3 BCD = vec3(-1, 1, 1);
  vec3 ACB = vec3(1, 1, -1);

  A.xyz = trns * A.xyz;
  B.xyz = trns * B.xyz;
  C.xyz = trns * C.xyz;
  D.xyz = trns * D.xyz;
  ACD = trns * ACD;
  ABD = trns * ABD;
  BCD = trns * BCD;
  ACB = trns * ACB;

  A += center;
  B += center;
  C += center;
  D += center;

  A = u_modelViewProjection * A;
  B = u_modelViewProjection * B;
  C = u_modelViewProjection * C;
  D = u_modelViewProjection * D;

  // ACD
  vec3 normal = normalize(ACD);
  color = min(litColor(normal, color) + emmisive, vec3(1));
  gl_Position = A;
  o_color = color;
  EmitVertex();
  gl_Position = D;
  EmitVertex();
  gl_Position = C;
  EmitVertex();
  EndPrimitive();

  // ABD
  normal = normalize(ABD);
  color = min(litColor(normal, color) + emmisive, vec3(1));
  gl_Position = A;
  o_color = color;
  EmitVertex();
  gl_Position = B;
  EmitVertex();
  gl_Position = D;
  EmitVertex();
  EndPrimitive();

  // BCD
  normal = normalize(BCD);
  color = min(litColor(normal, color) + emmisive, vec3(1));
  gl_Position = B;
  o_color = color;
  EmitVertex();
  gl_Position = C;
  EmitVertex();
  gl_Position = D;
  EmitVertex();
  EndPrimitive();

  // ACB
  normal = normalize(ACB);
  color = min(litColor(normal, color) + emmisive, vec3(1));
  gl_Position = A;
  o_color = color;
  EmitVertex();
  gl_Position = C;
  EmitVertex();
  gl_Position = B;
  EmitVertex();
  EndPrimitive();
}
