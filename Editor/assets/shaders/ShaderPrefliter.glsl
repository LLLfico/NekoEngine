#type vertex
#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;
layout (location = 5) in int a_entityId;

uniform mat4 u_view;
uniform mat4 u_projection;

struct vertexOutput{
	vec3 localPos;
};

layout (location = 0) out flat int v_entityId;
layout (location = 1) out vertexOutput outputs;

void main()
{
	outputs.localPos = a_position;
	vec4 pos = u_projection * u_view *  vec4(a_position, 1.0f);
	gl_Position = pos;
	v_entityId = a_entityId;
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;

const float PI = 3.14159265359;

struct vertexOutput{
	vec3 localPos;
};

layout (location = 0) in flat int v_entityId;
layout (location = 1) in vertexOutput inputs;

float RadicalInverse_Vdc(uint bits){
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N){
	return vec2(float(i) / float(N), RadicalInverse_Vdc(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness){
	float a = roughness * roughness;
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = normalize(cross(N, tangent));

	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
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

uniform samplerCube u_cubemap;
uniform float u_roughness;

void main()
{
	vec3 N = normalize(inputs.localPos);
	vec3 R = N;
	vec3 V = R;

	const uint SAMPLE_COUNT = 1024u;
	float totalWeight = 0.0;
	vec3 prefliteredColor = vec3(0.0);
	for (uint i = 0; i < SAMPLE_COUNT; i++){
		vec2 Xi = Hammersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(Xi, N, u_roughness);
		vec3 L = normalize(2.0 * dot(V, H) * H - V); // direction H recive light from direction L (reflection) 

		float NdotL = max(dot(N, L), 0.0);
		if (NdotL > 0.0){
			float D = DistributionGGX(N, H, u_roughness);
			float NdotH = max(dot(N, H), 0.0);
			float HdotV = max(dot(H, V), 0.0);
			float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

			float resolution = 512.0;
			float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
			float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float mipLevel = u_roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);


			prefliteredColor += textureLod(u_cubemap, L, mipLevel).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	prefliteredColor = prefliteredColor / totalWeight;
	o_color = vec4(prefliteredColor, 1.0f);
	o_entityId = v_entityId;
} 