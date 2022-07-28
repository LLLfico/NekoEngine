#type vertex
#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;
layout (location = 5) in int a_entityId;

uniform mat4 u_viewProjection;

struct vertexOutput{
	vec3 texcoord;
};

layout (location = 0) out flat int v_entityId;
layout (location = 1) out vertexOutput outputs;

void main()
{
	outputs.texcoord = a_position;
	vec4 pos = u_viewProjection * vec4(a_position, 1.0f);
	gl_Position = pos.xyww;
	v_entityId = a_entityId;
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;


struct vertexOutput{
	vec3 texcoord;
};

layout (location = 0) in flat int v_entityId;
layout (location = 1) in vertexOutput inputs;

uniform samplerCube u_cubemap;

void main()
{
	o_color = texture(u_cubemap, inputs.texcoord);
	o_entityId = v_entityId; 
} 