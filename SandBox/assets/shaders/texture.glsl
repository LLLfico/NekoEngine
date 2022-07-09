#type vertex
#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

uniform mat4 u_viewProjection;
uniform mat4 u_transform;

out vec3 v_normal;
out vec2 v_texcoord;

void main()
{
	v_normal = a_normal;
	v_texcoord = a_texcoord;
	gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);
}

#type fragment
#version 460 core
layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec2 v_texcoord;

uniform sampler2D u_texture;

void main()
{
	color = texture(u_texture, v_texcoord);
}