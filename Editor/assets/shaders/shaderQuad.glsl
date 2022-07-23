#type vertex
#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;
layout (location = 3) in float a_texIndex;
layout (location = 4) in float a_tilingFactor;
layout (location = 5) in int a_entityId;

layout (std140, binding = 0) uniform camera{
	mat4 u_viewProjection;
};
// uniform mat4 u_viewProjection;

struct vertexOutput{
	vec4 color;
	vec2 texCoord;
	float tilingFactor;
};

layout (location = 0) out vertexOutput outputs;
layout (location = 3) out flat float v_texIndex;
layout (location = 4) out flat int v_entityId;

void main()
{
	outputs.color = a_color;
	outputs.texCoord = a_texCoord;
	outputs.tilingFactor = a_tilingFactor;
	v_texIndex = a_texIndex;
	v_entityId = a_entityId;
	gl_Position = u_viewProjection * vec4(a_position, 1.0);
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;

struct vertexOutput{
	vec4 color;
	vec2 texCoord;
	float tilingFactor;
};

layout (location = 0) in vertexOutput inputs;
layout (location = 3) in flat float v_texIndex;
layout (location = 4) in flat int v_entityId;

layout (binding = 0) uniform sampler2D u_texture[32];

void main()
{
	// color = texture(u_texture, v_texCoord);
	vec4 texColor = inputs.color;
	switch(int(v_texIndex)){
		case 0: texColor *= texture(u_texture[0], inputs.texCoord * inputs.tilingFactor); break;
		case 1: texColor *= texture(u_texture[1], inputs.texCoord * inputs.tilingFactor); break;
		case 2: texColor *= texture(u_texture[2], inputs.texCoord * inputs.tilingFactor); break;
		case 3: texColor *= texture(u_texture[3], inputs.texCoord * inputs.tilingFactor); break;
		case 4: texColor *= texture(u_texture[4], inputs.texCoord * inputs.tilingFactor); break;
		case 5: texColor *= texture(u_texture[5], inputs.texCoord * inputs.tilingFactor); break;
		case 6: texColor *= texture(u_texture[6], inputs.texCoord * inputs.tilingFactor); break;
		case 7: texColor *= texture(u_texture[7], inputs.texCoord * inputs.tilingFactor); break;
		case 8: texColor *= texture(u_texture[8], inputs.texCoord * inputs.tilingFactor); break;
		case 9: texColor *= texture(u_texture[9], inputs.texCoord * inputs.tilingFactor); break;
		case 10: texColor *= texture(u_texture[10], inputs.texCoord * inputs.tilingFactor); break;
		case 11: texColor *= texture(u_texture[11], inputs.texCoord * inputs.tilingFactor); break;
		case 12: texColor *= texture(u_texture[12], inputs.texCoord * inputs.tilingFactor); break;
		case 13: texColor *= texture(u_texture[13], inputs.texCoord * inputs.tilingFactor); break;
		case 14: texColor *= texture(u_texture[14], inputs.texCoord * inputs.tilingFactor); break;
		case 15: texColor *= texture(u_texture[15], inputs.texCoord * inputs.tilingFactor); break;
		case 16: texColor *= texture(u_texture[16], inputs.texCoord * inputs.tilingFactor); break;
		case 17: texColor *= texture(u_texture[17], inputs.texCoord * inputs.tilingFactor); break;
		case 18: texColor *= texture(u_texture[18], inputs.texCoord * inputs.tilingFactor); break;
		case 19: texColor *= texture(u_texture[19], inputs.texCoord * inputs.tilingFactor); break;
		case 20: texColor *= texture(u_texture[20], inputs.texCoord * inputs.tilingFactor); break;
		case 21: texColor *= texture(u_texture[21], inputs.texCoord * inputs.tilingFactor); break;
		case 22: texColor *= texture(u_texture[22], inputs.texCoord * inputs.tilingFactor); break;
		case 23: texColor *= texture(u_texture[23], inputs.texCoord * inputs.tilingFactor); break;
		case 24: texColor *= texture(u_texture[24], inputs.texCoord * inputs.tilingFactor); break;
		case 25: texColor *= texture(u_texture[25], inputs.texCoord * inputs.tilingFactor); break;
		case 26: texColor *= texture(u_texture[26], inputs.texCoord * inputs.tilingFactor); break;
		case 27: texColor *= texture(u_texture[27], inputs.texCoord * inputs.tilingFactor); break;
		case 28: texColor *= texture(u_texture[28], inputs.texCoord * inputs.tilingFactor); break;
		case 29: texColor *= texture(u_texture[29], inputs.texCoord * inputs.tilingFactor); break;
		case 30: texColor *= texture(u_texture[30], inputs.texCoord * inputs.tilingFactor); break;
		case 31: texColor *= texture(u_texture[31], inputs.texCoord * inputs.tilingFactor); break;
	}
	o_color = texColor;
	o_entityId = v_entityId; 
} 