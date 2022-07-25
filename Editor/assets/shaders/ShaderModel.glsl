#type vertex
#version 460 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;
layout(location = 3) in vec4 a_color;
layout(location = 4) in int a_entityId;

uniform mat4 u_model;
uniform mat4 u_viewProjection;

struct vertexOutput{
	vec3 normal;
	vec2 texcoord;
	vec4 color;
};

layout(location = 0) out vertexOutput outputs;
layout(location = 3) out flat int v_entityId;

void main()
{
	outputs.normal = mat3(u_model) * a_normal;
	outputs.texcoord = a_texcoord;
	outputs.color = a_color;
	v_entityId = a_entityId;
	gl_Position = u_viewProjection * u_model * vec4(a_position, 1.0);
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;

struct vertexOutput{
	vec3 normal;
	vec2 texcoord;
	vec4 color;
};

layout(location = 0) in vertexOutput inputs;
layout(location = 3) in flat int v_entityId;

uniform sampler2D albedoMap;

void main()
{
	// color = texture(u_texture, v_texCoord);
	// o_color = inputs.color;
	o_color = texture(albedoMap, inputs.texcoord);
	// o_color = vec4(inputs.texcoord, 1.0f, 1.0f);
	o_entityId = v_entityId; 
} 