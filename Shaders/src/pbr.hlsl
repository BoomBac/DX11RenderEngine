struct PSInput
{
	float4 pos : SV_Position;
	float3 posW : WORLD_POSTION;
	float3 cameraPos : CAMERA_POSTION;
	float2 uv : Texcoord;
	float3 normal : NORMAL;
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
static const float PI = 3.14159265359;

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
float DistributionGGX(float3 N,float3 H,float roughness)
{
	float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
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
	float3 V = normalize(input.cameraPos - input.posW);
	float3 L = normalize(float3(0.f,50.f,0.f) - input.posW);
	float3 H = normalize(V + L);

	float3 radians = float3(1.f,1.f,1.f);//CalculateRadiance(-light_dir,-V,input.posW,H);
	 //basic reflection
 	float3 F0 = float3(0.04f, 0.04f, 0.04f);
 	F0 = lerp(F0, albedo, metallic);
	float NDF = DistributionGGX(input.normal, H, roughness);
	float3 F = FresnelSchlick(max(dot(H,V),0.f), F0);
	float G = GeometrySmith(input.normal,V,L,roughness);
	float3 L0 =float3(0.f,0.f,0.f);
	float3 kS = F;
 	float3 kD = float3(1.f, 1.f, 1.f) - kS;
 	kD *= 1.f - metallic;

 	float3 numerator    = NDF * G * F;
 	float denominator = 4.0 * max(dot(input.normal, V), 0.0) * max(dot(input.normal, L), 0.0)  + 0.0001;
 	float3 specular     = numerator / denominator;  
 	float NDotL = max(dot(input.normal,L),0.f);
 	L0 += (kD * albedo/PI + specular) * radians * NDotL;
	 	//ambient
	float4 color = float4(L0, 1.f);// + float4(0.1f,0.1f,0.1f,1.f);
 	//gamma correct
 	color = color / (color + float4(1.f,1.f,1.f,1.f));
 	color = pow(color,float4(1.f/2.2f,1.f/2.2f,1.f/2.2f,1.f/2.2f));
	return color;
		//float4(FresnelSchlick(max(dot(V, H), 0.f), float3(0.98f,0.56f,0.32f)),1.f);
		//GeometrySmith(input.normal, V, L, roughness);
		//DistributionGGX(input.normal,H, roughness);
}
// float4 main(PSInput input) : SV_TARGET
// {
// 	//surface normal
// 	float3 N = normalize(input.normal);
// 	//view dir
// 	float3 V = normalize(input.cameraPos - input.posW);


// 	// //caculate light
// 	float3 L = light_dir;
// 	//reflection radiance

// 	float3 H;

// 	//calculate Cook-Toorrance BRDF




// }