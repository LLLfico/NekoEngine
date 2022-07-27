#type vertex
#version 460 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;
layout(location = 3) in int a_entityId;

uniform mat4 u_model;
uniform mat4 u_viewProjection;

struct vertexOutput{
	vec3 worldPos;
	vec3 normal;
	vec2 texcoord;
};

layout(location = 0) out vertexOutput outputs;
out flat int v_entityId;

void main()
{
	outputs.worldPos = vec3(u_model * vec4(a_position, 1.0));
	outputs.normal = mat3(u_model) * a_normal;
	outputs.texcoord = a_texcoord;
	v_entityId = a_entityId;
	gl_Position = u_viewProjection * vec4(outputs.worldPos, 1.0f);
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;

struct vertexOutput{
	vec3 worldPos;
	vec3 normal;
	vec2 texcoord;
};

layout(location = 0) in vertexOutput inputs;
in flat int v_entityId;

// lights
// directional lights
struct DirectionalLight{
	vec3 direction;
	vec3 radiance;
};
uniform DirectionalLight u_directionallight;
// point lights
#define MAX_POINT_LIGHT_NUM 8
struct PointLight{
	vec3 position;
	vec3 radiance;
};
uniform PointLight u_pointlights[MAX_POINT_LIGHT_NUM];
uniform int u_pointLightsNum;

vec3 CaculateDirectionalLight(vec3 color, vec3 normal){
	vec3 lightdir = normalize(-u_directionallight.direction);
	float diff = max(dot(lightdir, normal), 0.0);
	return diff * u_directionallight.radiance * color;
}
vec3 CaculatePointLights(vec3 color, vec3 normal, vec3 worldPos){
	vec3 res = vec3(0.0f);
	for (int i = 0; i < u_pointLightsNum; i++){
		PointLight pointlight = u_pointlights[i];
		vec3 lightdir = normalize(pointlight.position - worldPos);
		float diff = max(dot(lightdir, normal), 0.0);
		res += diff * pointlight.radiance * color;
	}
	return res;
}

// shadow 
uniform sampler2D u_shadowMap;
uniform mat4 u_lightSpaceMatrix;
float CaculateShadowVisibility(vec3 fragWorldPos){
	vec4 lightSpacePos = u_lightSpaceMatrix * vec4(fragWorldPos, 1.0f);
	vec3 lightProjCoords = lightSpacePos.xyz / lightSpacePos.w;
	lightProjCoords = lightProjCoords * 0.5 + 0.5;
	float currentDepth = lightProjCoords.z;

	float visibility = 0.0f;
	vec2 texSize = 1.0f / textureSize(u_shadowMap, 0);
	int pcfRange = 3;
	for (int x = -pcfRange; x <= pcfRange; x++){
		for (int y = -pcfRange; y <= pcfRange; y++){
			float depth = texture(u_shadowMap, lightProjCoords.xy + vec2(x, y) * texSize).r;
			visibility += currentDepth - 0.005f > depth ? 0.0f : 1.0f;
		}
	}
	visibility /= float((pcfRange*2 + 1) * (pcfRange*2 + 1));

	return visibility;
}

uniform sampler2D u_albedoMap;

void main()
{
	vec4 color = texture(u_albedoMap, inputs.texcoord);
	// vec4 color = texture(u_shadowMap, inputs.texcoord);
	vec3 normal = normalize(inputs.normal);

	vec3 res = vec3(0.0f);
	res += CaculateDirectionalLight(color.xyz, normal);
	res += CaculatePointLights(color.xyz, normal, inputs.worldPos);
	
	float visibility = CaculateShadowVisibility(inputs.worldPos);
	// o_color = vec4(vec3(currentDepth), 1.0f);
	o_color = vec4(res * visibility, 1.0f);
	o_entityId = v_entityId; 
} 