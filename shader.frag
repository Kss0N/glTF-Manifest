#version 460

layout(location = 0) out vec4 fragColor;

layout(location = 0) in VData
{
	vec3 Pos;
	vec3 Normal;
	vec2 Tex0;
};


layout(location = 0) uniform vec4 u_baseColor = vec4(1, 1, 1, 1);
layout(location = 1) uniform int u_bUseBaseColorTexture;
layout(binding = 0) uniform sampler2D baseColorTexture;


void main(void)
{
	fragColor = u_baseColor * texture(baseColorTexture, Tex0);
}