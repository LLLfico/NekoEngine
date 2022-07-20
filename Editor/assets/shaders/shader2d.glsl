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
	vec4 texColor = v_color;
	switch(int(v_texIndex)){
		case 0: texColor *= texture(u_texture[0], v_texCoord * v_tilingFactor); break;
		case 1: texColor *= texture(u_texture[1], v_texCoord * v_tilingFactor); break;
		case 2: texColor *= texture(u_texture[2], v_texCoord * v_tilingFactor); break;
		case 3: texColor *= texture(u_texture[3], v_texCoord * v_tilingFactor); break;
		case 4: texColor *= texture(u_texture[4], v_texCoord * v_tilingFactor); break;
		case 5: texColor *= texture(u_texture[5], v_texCoord * v_tilingFactor); break;
		case 6: texColor *= texture(u_texture[6], v_texCoord * v_tilingFactor); break;
		case 7: texColor *= texture(u_texture[7], v_texCoord * v_tilingFactor); break;
		case 8: texColor *= texture(u_texture[8], v_texCoord * v_tilingFactor); break;
		case 9: texColor *= texture(u_texture[9], v_texCoord * v_tilingFactor); break;
		case 10: texColor *= texture(u_texture[10], v_texCoord * v_tilingFactor); break;
		case 11: texColor *= texture(u_texture[11], v_texCoord * v_tilingFactor); break;
		case 12: texColor *= texture(u_texture[12], v_texCoord * v_tilingFactor); break;
		case 13: texColor *= texture(u_texture[13], v_texCoord * v_tilingFactor); break;
		case 14: texColor *= texture(u_texture[14], v_texCoord * v_tilingFactor); break;
		case 15: texColor *= texture(u_texture[15], v_texCoord * v_tilingFactor); break;
		case 16: texColor *= texture(u_texture[16], v_texCoord * v_tilingFactor); break;
		case 17: texColor *= texture(u_texture[17], v_texCoord * v_tilingFactor); break;
		case 18: texColor *= texture(u_texture[18], v_texCoord * v_tilingFactor); break;
		case 19: texColor *= texture(u_texture[19], v_texCoord * v_tilingFactor); break;
		case 20: texColor *= texture(u_texture[20], v_texCoord * v_tilingFactor); break;
		case 21: texColor *= texture(u_texture[21], v_texCoord * v_tilingFactor); break;
		case 22: texColor *= texture(u_texture[22], v_texCoord * v_tilingFactor); break;
		case 23: texColor *= texture(u_texture[23], v_texCoord * v_tilingFactor); break;
		case 24: texColor *= texture(u_texture[24], v_texCoord * v_tilingFactor); break;
		case 25: texColor *= texture(u_texture[25], v_texCoord * v_tilingFactor); break;
		case 26: texColor *= texture(u_texture[26], v_texCoord * v_tilingFactor); break;
		case 27: texColor *= texture(u_texture[27], v_texCoord * v_tilingFactor); break;
		case 28: texColor *= texture(u_texture[28], v_texCoord * v_tilingFactor); break;
		case 29: texColor *= texture(u_texture[29], v_texCoord * v_tilingFactor); break;
		case 30: texColor *= texture(u_texture[30], v_texCoord * v_tilingFactor); break;
		case 31: texColor *= texture(u_texture[31], v_texCoord * v_tilingFactor); break;
	}
	color = texColor;
	color2 = v_entityId; 
} 