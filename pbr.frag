#version 460

layout(location = 0) out vec4 fragColor;

layout(location = 0) in VData
{
	vec3 Pos;
	vec3 Normal;
	vec2 Tex0;
};


layout(location = 1, std140) uniform CameraLightBlock
{
	vec3 camPos;
	vec3 lightPos;
	vec4 lightColor;
};



layout(location = 0) uniform vec4 u_baseColorFactor = vec4(1, 1, 1, 1);
layout(location = 1) uniform bool u_hasBaseColorTexture = false;

layout(location = 2) uniform vec2 u_metallicRoughnessFactor = vec2(1,1); //x := metallic Factor, y := roughness Factor
layout(location = 3) uniform bool u_hasMetallicRoughnessTexture = false;

layout(location = 4) uniform vec3 u_normalScale = vec3(1,1,1);
layout(location = 5) uniform bool u_hasNormalTexture = false;

layout(binding = 0) uniform sampler2D baseColorTexture;
layout(binding = 1) uniform sampler2D metallicRoughnessTexture;
layout(binding = 2) uniform sampler2D normalTexture;


/*
BRDF:

material = mix (dialectic_brdf, metal_brdf, metallic)

where:
	metal_brdf = conductor_fresnel(
		f0 = baseColor, 
		bsdf = specular_brdf(
			a = roughness**2) 
		)
	dialectic_brdf = fresnel_mix(
		ior = 1.5, [*1]
		base = diffuse_brdf(
			color = baseColor),
		layer = specular_brdf(a = roughness**2)
	) 
		


[*1] extension GLTF_KHR_ior will make it possible to supply IOR
*/

//using 
//	* throwdbridge-Reitz/GGX microfacet distribution
//	* seperable Smith joint shadowing function

struct DotProducts
{
	float
		VdL,
		VdN,
		VdH,
		LdN,
		LdH,
		NdH;
} dp;
const float PI = 3.141592;

float Chi(float x)
{
	return (x > 0) ? 1 : 0;
}

float specular_brdf (float a)
{
	float
		a2 = pow(a, 2),


		D =  
			a2 * Chi(dp.NdH)
			/
			(PI * pow( pow(dp.NdH, 2)*(a2-1) + 1, 2) )
		,

		V1 = 

			Chi(dp.LdH)
			/
			(dp.LdN + sqrt(a2 + (1-a2)*pow(dp.LdN, 2)))
		,
		
		V2 = 

			Chi(dp.VdH)
			/
			(dp.LdN * sqrt(a2 + (1-a2)*pow(dp.VdN, 2)))
	;
	return V1 * V2 * D;
}

vec3 diffuse_brdf(vec3 color)
{
	return (1 / PI) * color;
}

vec3 conductor_fresnel(vec3 f0, float bsdf, float _1_minus_VdH_pow5 )
{
	return bsdf * (f0 + (1-f0) * _1_minus_VdH_pow5);
}

vec3 fresnel_mix(float ior, vec3 base, float layer, float _1_minus_VdH_pow5)
{
	float 
		f0 = pow( (1-ior)/(1+ior), 2),
		fr = f0 + (1 - f0)*_1_minus_VdH_pow5
	;

	return mix(base, layer.xxx, fr);
}



void main(void)
{

	
	vec3
		  normal = u_hasNormalTexture ? u_normalScale * texture(normalTexture, Tex0).xyz : Normal,
		 viewDir = normalize(camPos - Pos),
		lightDir = normalize(lightPos - Pos),
		 halfDir = normalize(viewDir + lightDir);
	
	dp.VdL = dot( viewDir, lightDir);
	dp.VdN = dot( viewDir,   normal);
	dp.VdH = dot( viewDir,  halfDir);
	dp.LdN = dot(lightDir,   normal);
	dp.LdN = dot(lightDir,  halfDir);
	dp.LdN = dot(  normal,  halfDir);

	vec4
		baseColor = u_baseColorFactor * (u_hasBaseColorTexture ? texture(baseColorTexture, Tex0) : vec4(1,1,1,1)).xyzw;
	float 
		metallic = u_metallicRoughnessFactor.x * (u_hasMetallicRoughnessTexture ? texture(metallicRoughnessTexture, Tex0) : vec4(1,1,1,1)).b,
		roughness= u_metallicRoughnessFactor.y * (u_hasMetallicRoughnessTexture ? texture(metallicRoughnessTexture, Tex0) : vec4(1,1,1,1)).g,

		r2 = pow(roughness, 2),
		spec = specular_brdf(r2),
		_1_minus_VdH_pow5 = pow(1-dp.VdH, 5 ),

		ior = 1.5;

		


	vec3 
		metal_brdf = conductor_fresnel(baseColor.rgb, spec, _1_minus_VdH_pow5), 
		dialectic_brdf = fresnel_mix(ior, baseColor.rgb, spec, _1_minus_VdH_pow5),	
		
		material = mix(dialectic_brdf, metal_brdf, metallic);



	fragColor = vec4(material, 1.0f);
}