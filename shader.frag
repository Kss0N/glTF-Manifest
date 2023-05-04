#version 460

layout(location = 0) out vec4 fragColor;

layout(location = 0) in VData
{
	vec3 pos;
	vec3 normal;
	vec2 tex0;
};


layout(location = 0) uniform vec4 u_baseColor = vec4(1, 1, 1, 1);

uniform sampler2D baseColorTexture;


void main(void)
{
	fragColor = u_baseColor * texture(baseColorTexture, tex0);
}