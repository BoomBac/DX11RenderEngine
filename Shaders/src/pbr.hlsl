#include "common.hlsli"

struct PSInput
{
	float4 pos : SV_Position;
	float3 posW : WORLD_POSTION;
	float3 cameraPos : CAMERA_POSTION;
	float2 uv : Texcoord;
	float3 normal : NORMAL;
	matrix<float,3,3> btn : BTN;
};
cbuffer LightBuffer : register(b0)
{
	float3 light_pos;
	float light_intensity;
	float3 light_dir;
	float padding;
	float4 light_color;
	float affect_radius;
	float inner_angle_;
	float outer_angle;
	float light_type;
};

cbuffer LightMatrix : register(b1)
{
	row_major matrix light_view_projection;
	row_major matrix light_view;
	row_major matrix light_projection;
}

cbuffer MaterialProperty : register(b2)
{
	//Reflectance ratio
	float3 albedo;
	float metallic;
	float roughness;
	float3 padding_1;
}

//static const float3 albedo = float3(1.f,1.f,1.f);
//static const float metallic = 0.f;
//static const float roughness = 1.f;
static const float ao;
Texture2D diffuse_map : TEXTURE: register(t0);
Texture2D metallic_map : TEXTURE: register(t1);
Texture2D roughness_map : TEXTURE: register(t2);
Texture2D albedo_map : TEXTURE: register(t3);
Texture2D normal_map : TEXTURE: register(t4);
Texture2D ao_map : TEXTURE: register(t5);
TextureCube irradiance_map : register(t6);
TextureCube env_map : register(t7);



SamplerState objSamplerState : SAMPLER;

float3 CalculateRadiance(float3 l_dir,float3 v_dir,float3 p_w)
{
	//calculate attenuation
	float attenuation = 1.f;
	float3 radians = light_color * attenuation;
	return radians;
}
////calculate the radio of reflect
////input dot(half-vector,view_dir)
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  
float3 FresnelSchlikRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + float3(max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * 
		pow(float3(clamp(float3(1.0 - cosTheta, 1.0 - cosTheta, 1.0 - cosTheta), float3(0.f, 0.f, 0.f), float3(1.0, 1.0, 1.0))), float3(5.f, 5.f, 5.f));
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

float4 main(PSInput input) : SV_TARGET
{
	//	return float4(1.f,1.f,1.f,1.f);
	//float3 albedo = float3(1.f,0.86f,0.76f);//pow(albedo_map.Sample(objSamplerState,input.uv).rgb, 2.2);
	float3 N = GetNormal(normal_map, input.uv, objSamplerState, input.btn);
	//float metallic = metallic_map.Sample(objSamplerState, input.uv).r;
	//float roughness = roughness_map.Sample(objSamplerState, input.uv).r;
	float3 D = diffuse_map.Sample(objSamplerState, input.uv);
	//float ao = texture(ao_map, input.uv).r;

	float3 V = normalize(input.cameraPos - input.posW);
	float3 L = normalize(light_pos - input.posW);
	float3 H = normalize(V + L);

	float3 radians = light_color * light_intensity* max(0.f, dot(normalize(N), normalize(light_pos - input.posW))) *
		clamp(1 - pow(distance(input.posW, light_pos) / affect_radius, 4.f), 0.f, 2.f);
	//float3 radians = light_intensity * ;//CalculateRadiance(-light_dir,-V,input.posW,H);
	//basic reflection
 	float3 F0 = float3(0.04f, 0.04f, 0.04f);
 	F0 = lerp(F0, albedo, metallic);
	float NDF = DistributionGGX(N, H, roughness);
	float3 F = FresnelSchlick(max(dot(H,V),0.f), F0);
	float G = GeometrySmith(N,V,L,roughness);
	float3 L0 =float3(0.f,0.f,0.f);
	float3 kS = F;
 	float3 kD = float3(1.f, 1.f, 1.f) - kS;
 	kD *= 1.f - metallic;

 	float3 numerator    = NDF * G * F;
 	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
 	float3 specular     = numerator / denominator;  
 	float NDotL = max(dot(N,L),0.f);
 	L0 += (kD * albedo/PI + specular) * radians * NDotL;
	//calculate ambient
	float3 kS_e = FresnelSchlikRoughness(max(dot(H, V), 0.f), F0, roughness);
	float3 kD_e = float3(1.f, 1.f, 1.f) - kS_e;
	float3 diffuse = irradiance_map.Sample(objSamplerState, N).rgb * albedo * kD_e;
	float3 ambient = diffuse;


	float3 color = L0 + ambient * 1.f;
 	//gamma correct
 	color = color / (color + float3(1.f,1.f,1.f));
 	color = pow(color,float3(1.f/2.2f,1.f/2.2f,1.f/2.2f));
	return float4(color, 1.f);
}