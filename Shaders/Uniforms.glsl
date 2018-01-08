
layout (std140) uniform ShaderActivationUniforms
{
  mat4 u_projection;                // 64
  mat4 u_view;                      // 64
  int u_directionalLightsCount;     // 4
  int u_pointLightsCount;           // 4
  vec3 u_eyePos;                    // 12
  float u_fogFar;                   // 4
  float u_fogNear;                  // 4
  float u_fogExp;                   // 4
  float u_time;                     // 4
  vec4 u_fogColorNear;              // 16
  vec4 u_fogColorFar;               // 16
};
