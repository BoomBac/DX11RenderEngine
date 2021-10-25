#include "common.hlsli"

TextureCube cube_map : register(t0);
SamplerState objSamplerState : SAMPLER;

struct PsIn
{
	float4 pos : SV_POSITION;
	float3 pos_w : WORLDPOSITION;
};

static const float rou = 0.f;

float RadicalInverse(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
float2 Hammersley(uint i, uint N)
{
    return float2(float(i)/float(N), RadicalInverse(i));
}  

float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 N)
{
    float a = Roughness * Roughness;
    float Phi = 2 * PI * Xi.x;
    float CosTheta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
    float SinTheta = sqrt(1 - CosTheta * CosTheta);
    float3 H;
    H.x = SinTheta * cos(Phi);
    H.y = SinTheta * sin(Phi);
    H.z = CosTheta;
    float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 TangentX = normalize(cross(UpVector, N));
    float3 TangentY = cross(N, TangentX);
    // Tangent to world space
    return TangentX * H.x + TangentY * H.y + N * H.z;
}
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
    return float4(PrefilterEnvMap(rou,normalize(pin.pos_w)),1.f);
}