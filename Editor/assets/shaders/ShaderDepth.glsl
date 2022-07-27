#type vertex
#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

out vec3 v_normal;

uniform mat4 u_model;
uniform mat4 u_viewProjection;

void main()
{
	v_normal = a_normal;
	gl_Position = u_viewProjection * u_model * vec4(a_position, 1.0f);
}


#type fragment
#version 460 core

layout (location = 0) out vec4 o_color;
layout (location = 1) out int o_entityId;

in vec3 v_normal;

void main()
{
	// o_color = vec4(v_normal, 1.0f);
	o_color = vec4(vec3(gl_FragCoord.z), 1.0f);
	// o_color = vec4(1.0f, 0.0f, 0.0, 1.0f);
	o_entityId = -1;
} 