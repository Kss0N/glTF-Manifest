#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex0;

layout(location = 0) out VData
{
	vec3 Pos;
	vec3 Normal;
	vec2 Tex0;
};

layout(binding = 0, std140) uniform MatrixBlock
{
	mat4 
		u_projection,
		u_view,
		u_model,
		u_normal;
};

void main (void)
{
	gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);

	Pos = vec3(u_model * vec4(aPos, 1.0));
	Normal = normalize(mat3(u_normal) * aNormal);
	Tex0 = aTex0;
}