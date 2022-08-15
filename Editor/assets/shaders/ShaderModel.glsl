#type vertex
#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;
layout (location = 5) in int a_entityId;

layout (location = 6) in ivec4 a_boneIds;
layout (location = 7) in vec4 a_weights;

uniform mat4 u_model;
uniform mat4 u_viewProjection;

uniform bool u_animated;

const int MAX_BONE_NUMS = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_finalBoneMatrices[MAX_BONE_NUMS];

struct vertexOutput{
	vec3 worldPos;
	vec3 normal;
	vec2 texcoord;
	vec3 tangent;
	vec3 bitangent;
};

layout (location = 0) out flat int v_entityId;
layout (location = 1) out vertexOutput outputs;

void main()
{
	outputs.texcoord = a_texcoord;
	outputs.tangent = a_tangent;
	outputs.bitangent = a_bitangent;
	v_entityId = a_entityId;
	if (u_animated){
		mat4 boneTransformMatrix = u_finalBoneMatrices[a_boneIds[0]] * a_weights[0];
			 boneTransformMatrix += u_finalBoneMatrices[a_boneIds[1]] * a_weights[1];
			 boneTransformMatrix += u_finalBoneMatrices[a_boneIds[2]] * a_weights[2];
			 boneTransformMatrix += u_finalBoneMatrices[a_boneIds[3]] * a_weights[3];
		mat3 normalMatrix = transpose(inverse(mat3(boneTransformMatrix)));
		outputs.normal = normalMatrix * a_normal;
		vec4 pos = u_model * boneTransformMatrix * vec4(a_position, 1.0f);
		outputs.worldPos = pos.xyz;
		gl_Position = u_viewProjection * vec4(outputs.worldPos, 1.0f);
	}
	else {
		outputs.worldPos = vec3(u_model * vec4(a_position, 1.0));
		outputs.normal = mat3(u_model) * a_normal;
		
		gl_Position = u_viewProjection * vec4(outputs.worldPos, 1.0f);
	}

	
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;

const float PI = 3.14159265359;

struct vertexOutput{
	vec3 worldPos;
	vec3 normal;
	vec2 texcoord;
	vec3 tangent;
	vec3 bitangent;
};

layout (location = 0) in flat int v_entityId;
layout (location = 1) in vertexOutput inputs;

// pbr
uniform sampler2D u_albedoMap;
uniform sampler2D u_metallicMap;
uniform sampler2D u_roughnessMap;
uniform sampler2D u_aoMap;

vec3 FresnelSchlick(float cosTheta, vec3 F0){
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0f);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0) - roughness, F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness){
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0- k) + k;

	return num / denom;
}

float GemotrySmith(vec3 N, vec3 V, vec3 L, float roughness){
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

// shadow (just directional light shadow currently)
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

vec3 CaculateDirectionalLight(vec3 color, vec3 normal, vec3 viewdir, float roughness, float metallic){
	vec3 Lo = vec3(0.0f);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, color, metallic);

	vec3 lightdir = normalize(-u_directionallight.direction);

	vec3 halfNormal = normalize(viewdir + lightdir);

	vec3 radiance = u_directionallight.radiance;
	
	vec3 F = FresnelSchlick(max(dot(halfNormal, viewdir), 0.0), F0);
	float D = DistributionGGX(normal, halfNormal, roughness);
	float G = GemotrySmith(normal, viewdir, lightdir, roughness);

	vec3 numerator = D * F * G;
	float denominator = 4.0 * max(dot(normal, viewdir), 0.0) * max(dot(normal, lightdir), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	vec3 ks = F;
	vec3 kd = vec3(1.0) - ks;
	kd *= 1.0 - metallic;

	float cosTheta = max(dot(normal, lightdir), 0.0);
	Lo = (kd * color / PI + ks * specular) * radiance * cosTheta;
	return Lo;
}
vec3 CaculatePointLights(vec3 color, vec3 normal, vec3 worldPos, vec3 viewdir, float roughness, float metallic){
	vec3 Lo = vec3(0.0f);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, color, metallic);

	for (int i = 0; i < u_pointLightsNum; i++){
		PointLight pointlight = u_pointlights[i];
		vec3 lightdir = normalize(pointlight.position - worldPos);
		vec3 halfNormal = normalize(viewdir + lightdir);

		float distance = length(pointlight.position - worldPos);
		float attenuation = distance < 1.0f ? 1.0f : 1.0f / (distance * distance);
		vec3 radiance = pointlight.radiance * attenuation;
		
		vec3 F = FresnelSchlick(max(dot(halfNormal, viewdir), 0.0), F0);
		float D = DistributionGGX(normal, halfNormal, roughness);
		float G = GemotrySmith(normal, viewdir, lightdir, roughness);

		vec3 numerator = D * F * G;
		float denominator = 4.0 * max(dot(normal, viewdir), 0.0) * max(dot(normal, lightdir), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		vec3 ks = F;
		vec3 kd = vec3(1.0) - ks;
		kd *= 1.0 - metallic;

		float cosTheta = max(dot(normal, lightdir), 0.0);
		Lo += (kd * color / PI + ks * specular) * radiance * cosTheta;
	}
	return Lo;
}

uniform vec3 u_cameraPos;
uniform samplerCube u_iradianceMap;
uniform samplerCube u_prefliterMap;
uniform sampler2D u_brdfLUT;

void main()
{
	vec4 basecolor = texture(u_albedoMap, inputs.texcoord);
	vec3 normal = normalize(inputs.normal);
	vec3 viewdir = normalize(u_cameraPos - inputs.worldPos);
	vec3 reflectionDir = reflect(-viewdir, normal);

	float roughness = texture(u_roughnessMap, inputs.texcoord).r;
	float metallic = texture(u_metallicMap, inputs.texcoord).r;
	float ao = texture(u_aoMap, inputs.texcoord).r;

	vec3 res = vec3(0.0f);

	vec3 F0 = vec3(0.04);
	vec3 F = FresnelSchlickRoughness(max(dot(normal, viewdir), 0.0), F0, roughness);
	vec3 ks = F;
	vec3 kd = vec3(1.0f) - ks;
	kd *= 1.0 - metallic;
	vec3 iradiance = texture(u_iradianceMap, normal).rgb;
	vec3 diffuse = iradiance * basecolor.rgb;

	const float MAX_REFLECTION_LOD = 4;
	vec3 prefliteredColor = textureLod(u_prefliterMap, reflectionDir, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture(u_brdfLUT, vec2(max(dot(normal, viewdir), 0.0), roughness)).rg;
	vec3 specular = prefliteredColor * (F * envBRDF.x + envBRDF.y);
	vec3 ambient = (kd * diffuse + specular) * ao; // F already has ks

	res += ambient;
	res += CaculateDirectionalLight(basecolor.xyz, normal, viewdir, roughness, metallic);
	res += CaculatePointLights(basecolor.xyz, normal, inputs.worldPos, viewdir, roughness, metallic);
	
	// hdr
	res = res / (res + vec3(1.0f));
	// gamma
	res = pow(res, vec3(1.0/2.2));

	float visibility = CaculateShadowVisibility(inputs.worldPos);
	// o_color = vec4(vec3(currentDepth), 1.0f);
	o_color = vec4(res * visibility, 1.0f);
	o_entityId = v_entityId; 
} 