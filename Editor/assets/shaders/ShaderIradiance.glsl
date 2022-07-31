#type vertex
#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;
layout (location = 5) in int a_entityId;

uniform mat4 u_view;
uniform mat4 u_projection;

struct vertexOutput{
	vec3 texcoord;
};

layout (location = 0) out flat int v_entityId;
layout (location = 1) out vertexOutput outputs;

void main()
{
	outputs.texcoord = a_position;
	vec4 pos = u_projection * u_view *  vec4(a_position, 1.0f);
	gl_Position = pos;
	v_entityId = a_entityId;
}


#type fragment
#version 460 core
layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityId;

const float PI = 3.14159265359;

struct vertexOutput{
	vec3 texcoord;
};

layout (location = 0) in flat int v_entityId;
layout (location = 1) in vertexOutput inputs;

uniform samplerCube u_cubemap;

void main()
{
	// for pre compute test
	vec3 irradiance = vec3(0.0f);
	
	vec3 normal = normalize(inputs.texcoord);
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 right = normalize(cross(up, normal)); // left ?
	up = normalize(cross(normal, right));

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta){
	 	for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta){
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z + normal;

			irradiance += texture(u_cubemap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	o_color = vec4(irradiance, 1.0f);
	o_entityId = v_entityId; 
} 