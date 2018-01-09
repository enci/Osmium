
#pragma include "Uniforms.glsl"

uniform mat4 u_model;
uniform mat4 u_modelViewProjection;
uniform vec3 u_ambient;
uniform vec3 u_diffuse;

const float kRimGamma = 4.0;
const float kLightRadius = 5.2;

uniform sampler2D s_shadowMaps[DIR_LIGHT_COUNT];

vec3 CalculateDirectionalLights(in vec3 normal)
{
  vec3 color = vec3(0.0, 0.0, 0.0);

  for(int i = 0; i < u_directionalLightsCount && i < DIR_LIGHT_COUNT; i++)
  {
    vec3 light_dir = u_directionalLights[i].direction;
    float intensity = max(0.0, dot(normal, light_dir));
    color += u_directionalLights[i].color * intensity * u_diffuse;
  }

  return color;
}

vec3 CalculateDirectionalLightsWithShadow(in vec3 position, in vec3 normal)
{
  vec3 color = vec3(0.0, 0.0, 0.0);

  for(int i = 0; i < u_directionalLightsCount && i < DIR_LIGHT_COUNT; i++)
  {
    float shadow = 1.0;
    if(u_directionalLights[i].castShadow == true)
    {
      vec4 posLightCoor = u_directionalLights[i].shadowMatrix * vec4(position, 1.0);
      vec3 projCoords = posLightCoor.xyz / posLightCoor.w;
      projCoords = projCoords * 0.5 + 0.5;

      float bias = 0.002;

      float closestDepth = texture(s_shadowMaps[i], projCoords.xy).r;   
      float currentDepth = projCoords.z;  
      shadow = abs(currentDepth - bias) > closestDepth  ? 0.0 : 1.0;
    }

    vec3 light_dir = u_directionalLights[i].direction;
    float intensity = max(0.0, dot(normal, light_dir)) * shadow;
    color += u_directionalLights[i].color * intensity * u_diffuse;
  }

  return color;
}

vec3 CalculatePointLights(in vec3 position, in vec3 normal)
{
  vec3 color = vec3(0.0, 0.0, 0.0);

  for(int i = 0; i < u_pointLightsCount && i < POINT_LIGHT_COUNT; i++)
  {
    vec3 light_dir = u_pointLights[i].position - position;
    float dist = length(light_dir);
    light_dir = normalize(light_dir);
    float intensity = max(0.0, dot(normal, light_dir));

    vec3 l = u_pointLights[i].position - position;
    float d = length(l);
    float attenuation = 1.0 - clamp((d * d) / (u_pointLights[i].radius * u_pointLights[i].radius), 0.0, 1.0);
    color += u_pointLights[i].color * intensity * attenuation * u_diffuse;
  }

  return color;
}

vec3 CalculateDirectionalLightsSpecular(
  in vec3 normal,
  in vec3 position,
  in float shininess)
{
  vec3 color = vec3(0.0, 0.0, 0.0);

  for(int i = 0; i < u_directionalLightsCount && i < DIR_LIGHT_COUNT; i++)
  {
    vec3 lightDir = u_directionalLights[i].direction;
    float diffuse = max(0.0, dot(normal, lightDir));
    color += u_directionalLights[i].color * diffuse * u_diffuse;

    vec3 viewDir = normalize(u_eyePos - position);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    color += u_directionalLights[i].color * specular;
  }

  return color;
}

vec3 CalculateRimLight(in vec3 normal)
{
  float rim = 1.0 - max(0.0, dot(normal, normalize(u_eyePos)));
  rim = pow(rim, kRimGamma) * 1.0;
  return u_diffuse * rim;
}

void AddFog(inout vec3 color, in vec3 viewPosition)
{
  float param = (u_fogFar - length(viewPosition)) / (u_fogFar - u_fogNear);
	param = clamp(1 - param, 0.0, 1.0);
	vec4 fogColor = mix(u_fogColorNear, u_fogColorFar, param);
	color = mix(color, fogColor.rgb, param * fogColor.a);
}
