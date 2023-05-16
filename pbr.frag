#version 460

layout(location = 0) out vec4 fragColor;

layout(location = 0) in VData
{
	vec3 pos;
	vec3 normal;
	vec2 tex0;
};


layout(location = 1, std140) uniform CameraLightBlock
{
	vec3 camPos;
	vec3 lightPos;
	vec4 lightColor;
};



layout(location = 0) uniform vec4 u_baseColor = vec4(1, 1, 1, 1);
layout(location = 1) uniform vec2 u_metallicRoughnessFactor = vec2(1,1);

layout(binding = 0) uniform sampler2D baseColorTexture;
layout(binding = 1) uniform sampler2D metallicRoughnessTexture;


void main(void)
{
	fragColor = u_baseColor * texture(baseColorTexture, tex0);
}