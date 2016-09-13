#version 430 core

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

const float kFogFar = 1500.0;
const float kFogNear = 80.0;
const float kRimGamma = 4.0;
const float kLightRadius = 5.2;
const float kFogGamma = 1.0f;

// Uniforms
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_modelViewProjection;
uniform int u_directionalLightsCount;
uniform int u_pointLightsCount;
uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_eyePos;

// Lights
#define DIR_LIGHT_COUNT     5
#define POINT_LIGHT_COUNT   10

// All the directional lights
uniform DirectionalLight    u_directionalLights[DIR_LIGHT_COUNT];
// All the point lights
uniform PointLight          u_pointLights[POINT_LIGHT_COUNT];

// Vertex atributes
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texture;

// Vertex shader outputs
out vec2 v_texture;
out vec3 v_color;


void main()
{
  vec3 fogColor = u_directionalLights[1].color * 0.5;

	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

	vec3 normal = normalize((u_model * vec4(a_normal, 0.0)).xyz);
	vec3 worldPosition = (u_model * vec4(a_position, 1.0)).xyz;
	v_color = u_ambient;

	vec3 vPos = (u_view * u_model * vec4(a_position, 1.0)).xyz;
  vec3 toEye = normalize(-vPos);
	vec3 vNormal =  normalize((u_view * u_model * vec4(a_normal, 0.0)).xyz);
	float rim = 1 - max(0.0, dot(vNormal, toEye));
  rim = pow(rim, kRimGamma);
  v_color += vec3(u_diffuse * rim);

	// Directional lights
	for(int i = 0; i < u_directionalLightsCount && i < DIR_LIGHT_COUNT; i++)
	{
		vec3 light_dir = u_directionalLights[i].direction;
		float intensity = max(0.0, dot(normal, light_dir));
		v_color += u_directionalLights[i].color * intensity;
	}

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

  float fog = (kFogFar - length(vPos)) / (kFogFar - kFogNear);
  fog = pow(fog, kFogGamma);
  v_color = mix(fogColor, v_color, fog);

	v_color = min(vec3(1.0), v_color * u_diffuse);

	//v_color = vec3(gl_Position.z / gl_Position.w);
}
