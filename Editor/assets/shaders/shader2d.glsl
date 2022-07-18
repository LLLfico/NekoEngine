#type vertex
#version 460 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in float a_texIndex;
layout(location = 4) in float a_tilingFactor;
layout(location = 5) in int a_entityId;

uniform mat4 u_viewProjection;

out vec4 v_color;
out vec2 v_texCoord;
flat out float v_texIndex;
out float v_tilingFactor;
out flat int v_entityId;

void main()
{
	v_color = a_color;
	v_texCoord = a_texCoord;
	v_texIndex = a_texIndex;
	v_tilingFactor = a_tilingFactor;
	v_entityId = a_entityId;
	gl_Position = u_viewProjection * vec4(a_position, 1.0);
}


#type fragment
#version 460 core
layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_color;
in vec2 v_texCoord;
flat in float v_texIndex;
in float v_tilingFactor;
in flat int v_entityId;

uniform sampler2D u_texture[32];

void main()
{
	// color = texture(u_texture, v_texCoord);
	color = texture(u_texture[int(v_texIndex)], v_texCoord * v_tilingFactor) * v_color;
	color2 = v_entityId; 
} 