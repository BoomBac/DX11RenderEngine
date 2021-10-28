#include "common.hlsli"

TextureCube cube_map : register(t0);
SamplerState objSamplerState : SAMPLER;

struct PsIn
{
	float4 pos : SV_POSITION;
	float3 pos_w : WORLDPOSITION;
};

cbuffer buffer
{
    float c_roughness;
};

static const float rou = 0.f;

float3 PrefilterEnvMap( float Roughness, float3 R ) 
{
    float3 N = R; 
    float3 V = R;
    float3 PrefilteredColor = 0;
    const uint NumSamples = 1024; 
    float TotalWeight = 0.f;

    for( uint i = 0; i < NumSamples; i++ ) 
    {
        float2 Xi = Hammersley( i, NumSamples );   
        float3 H = ImportanceSampleGGX( Xi, Roughness, N ); 
        float3 L = 2 * dot( V, H ) * H - V;
        float NoL = saturate( dot( N, L ) ); 
        if( NoL > 0 ) 
        {
            float D = DistributionGGX(N,H,Roughness);
            float HoV = saturate(dot(H,V));
            float NoH = saturate(dot(N,H));
            float pdf = (D * NoH / (4.f * HoV)) + 0.0001f;
            float resolution = 1024;
            float saTexel = 4.f * PI / (6.f * resolution * resolution);
            float saSample = 1.f / (float(NumSamples) * pdf + 0.0001f);
            float mipLevel = Roughness == 0.0f? 0.0 : 0.5 * log2(saSample / saTexel);
            PrefilteredColor += cube_map.SampleLevel(objSamplerState,L, mipLevel).rgb * NoL;
            //PrefilteredColor += cube_map.Sample(objSamplerState, L).rgb * NoL;
            TotalWeight += NoL;
        }
    } 
    return PrefilteredColor / TotalWeight;
}

float4 main(PsIn pin) : SV_TARGET
{
    return float4(PrefilterEnvMap(c_roughness,normalize(pin.pos_w)),1.f);
}