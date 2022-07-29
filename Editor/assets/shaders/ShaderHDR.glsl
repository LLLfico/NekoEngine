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
	gl_Position = u_projection * u_view * vec4(a_position, 1.0f);
	v_entityId = a_entityId;
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;


struct vertexOutput{
	vec3 localPos;
};

layout (location = 0) in flat int v_entityId;
layout (location = 1) in vertexOutput inputs;

uniform sampler2D u_equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v){
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(inputs.localPos));
	vec3 color = texture(u_equirectangularMap, uv).rgb;
	o_color = vec4(color, 1.0f);
	o_entityId = v_entityId; 
} 