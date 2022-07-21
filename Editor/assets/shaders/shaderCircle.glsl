#type vertex
#version 460 core
layout(location = 0) in vec3 a_worldPosition;
layout(location = 1) in vec3 a_localPosition;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_thickness;
layout(location = 4) in float a_fade;
layout(location = 5) in int a_entityId;

uniform mat4 u_viewProjection;

struct vertexOutput{
	vec3 localPosition;
	vec4 color;
	float thickness;
	float fade;
};

layout (location = 0) out vertexOutput outputs;
layout (location = 4) out flat int v_entityId;

void main()
{
	outputs.localPosition = a_localPosition;
	outputs.color = a_color;
	outputs.thickness = a_thickness;
	outputs.fade = a_fade;

	v_entityId = a_entityId;

	gl_Position = u_viewProjection * vec4(a_worldPosition, 1.0);
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;

struct vertexOutput{
	vec3 localPosition;
	vec4 color;
	float thickness;
	float fade;
};
layout (location = 0) in vertexOutput inputs;
layout (location = 4) in flat int v_entityId;

void main()
{
	float distance = 1.0 - length(inputs.localPosition);
	float circle = smoothstep(0.0, inputs.fade, distance);
	circle *= smoothstep(inputs.thickness + inputs.fade, inputs.thickness, distance);
	
	if (circle == 0) {
		discard;
	}
	
	o_color = inputs.color;
	o_color.a *= circle;
	o_entityId = v_entityId; 
} 