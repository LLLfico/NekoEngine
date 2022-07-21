#type vertex
#version 460 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in int a_entityId;

uniform mat4 u_viewProjection;

struct vertexOutput{
	vec4 color;
};

layout(location = 0) out vertexOutput outputs;
layout(location = 1) out flat int v_entityId;

void main()
{
	outputs.color = a_color;
	v_entityId = a_entityId;

	gl_Position = u_viewProjection * vec4(a_position, 1.0);
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;

struct vertexOutput{
	vec4 color;
};

layout(location = 0) in vertexOutput inputs;
layout(location = 1) in flat int v_entityId;

void main()
{
	o_color = inputs.color;
	o_entityId = v_entityId; 
} 