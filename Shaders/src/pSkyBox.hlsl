TextureCube objTexture : register(t0);
SamplerState objSamplerState : SAMPLER;

struct PsIn
{
	float4 pos : SV_POSITION;
	float3 pos_w : WORLDPOSTION;
};

float4 main(PsIn pin) : SV_TARGET
{
	return float4(objTexture.Sample(objSamplerState,pin.pos_w).xyz,1.f);
}