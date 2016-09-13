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

const float kLightRadius = 4.2;



// Uniforms
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_modelViewProjection;
uniform int u_directionalLightsCount;
uniform int u_pointLightsCount;

// Lights
#define DIR_LIGHT_COUNT     5
#define POINT_LIGHT_COUNT   5

//
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
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

	vec3 normal = normalize((u_model * vec4(a_normal, 0.0)).xyz);

	vec3 worldPosition = (u_model * vec4(a_position, 1.0)).xyz;

	v_color = vec3(0.0, 0.0, 0.0);

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
		float attenuation = 1.0 / (1.0 + (dist / kLightRadius));
		v_color += u_pointLights[i].color * intensity * attenuation;
	}

	v_color = min(vec3(1.0), v_color);

	//v_color = vec3(gl_Position.z / gl_Position.w);
}
