// Lights
#define DIR_LIGHT_COUNT     5
#define POINT_LIGHT_COUNT   10

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    bool castShadow;
    mat4 shadowInvTransform;
    //sampler2D shadowMap;
};

struct PointLight
{
    vec3 position;
    vec3 color;
    float radius;
};

// Uniforms
uniform ShaderActivationUniforms
{
  mat4 u_projection;
  mat4 u_view;
  int u_directionalLightsCount;
  int u_pointLightsCount;
  vec3 u_eyePos;  
  float u_fogFar;
  float u_fogNear;
  float u_fogExp;
  float u_time;
  vec4 u_fogColorNear;
  vec4 u_fogColorFar;

  // All the directional lights
  DirectionalLight u_directionalLights[DIR_LIGHT_COUNT];

  // All the point lights
  PointLight u_pointLights[POINT_LIGHT_COUNT];
};

uniform DrawCallUniforms
{
  mat4 u_model;
  mat4 u_modelViewProjection;
  vec3 u_ambient;
  vec3 u_diffuse;
};