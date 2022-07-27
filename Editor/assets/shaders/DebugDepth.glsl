#type vertex
#version 460 core
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texcoord;

out vec2 v_texcoord;

void main()
{
	v_texcoord = a_texcoord;
	gl_Position = vec4(a_position.x, a_position.y, 0.0f, 1.0f);
}


#type fragment
#version 460 core

layout (location = 0) out vec4 o_color;

in vec2 v_texcoord;

uniform sampler2D u_shadowMap;

void main()
{
	float depth = texture(u_shadowMap, v_texcoord).r;
	o_color = vec4(vec3(depth), 1.0f);
	// o_color = texture(u_shadowMap, v_texcoord);
	// o_color = vec4(v_texcoord.x, v_texcoord.y, 0.0f, 1.0f);
} 