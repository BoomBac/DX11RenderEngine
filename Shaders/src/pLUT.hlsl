
#include "Pcommon.hlsli"

float GeometrySchlickGGXForIBL(float NdotV, float roughness)
{
	float a = roughness;
	float k = (a * a) / 2.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
float GeometrySmithForIBL(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGXForIBL(NdotV, roughness);
	float ggx1 = GeometrySchlickGGXForIBL(NdotL, roughness);

	return ggx1 * ggx2;
}

//calculate LUT
float2 IntegrateBRDF(float NoV,float roughness)
{
	float3 V;
	V.x = sqrt(1.0f - NoV * NoV); //sin 
	V.y = 0; 
	V.z = NoV;
	// cos
	float A = 0;
	float B = 0;
	float3 N = float3(0.f,0.f,1.f);
	const int NumSamples = 1024;
	for (int i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi, roughness, N);
		float3 L = 2 * dot(V, H) * H - V;
		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V, H));
		if (NoL > 0)
		{
			//float3 N, float3 V, float3 L, float roughness
			float G = GeometrySmithForIBL(N, V, L, roughness);
			float G_Vis = G * VoH / (NoH * NoV);
			float Fc = pow(1 - VoH, 5);
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	return float2(A, B) / NumSamples;
}

float4 main(float2 uv : TEXCOORD0) : SV_TARGET
{
	return float4(IntegrateBRDF(uv.x,uv.y),0.0f, 1.0f);
}