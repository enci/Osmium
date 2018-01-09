
// Lights
#define DIR_LIGHT_COUNT     5
#define POINT_LIGHT_COUNT   16

struct DirectionalLight           
{
    vec3 direction;               // 12
    bool castShadow;              // 4
    vec3 color;                   // 12
    float _padding;               // 4
    mat4 shadowMatrix;            // 64
};                                // Total 96

struct PointLight
{
    vec3 position;                // 12
    float radius;                 // 4
    vec3 color;                   // 12
    float _padding;               // 4
};                                // Total 32

layout (std140) uniform ShaderActivationUniforms
{
  mat4 u_projection;                // 64   64
  mat4 u_view;                      // 64   128
  vec3 u_eyePos;                    // 12   140
  int u_directionalLightsCount;     // 4    144

  //int u_pointLightsCount;           // 4  
  //float u_fogFar;                   // 4
  //float u_fogNear;                  // 4
  //float u_fogExp;                   // 4
  //float u_time;                     // 4
  //vec4 u_fogColorNear;              // 16
  //vec4 u_fogColorFar;               // 16
};  // PerShaderUBO
