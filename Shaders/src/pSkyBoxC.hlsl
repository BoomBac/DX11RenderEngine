TextureCube cube_map : register(t0);
SamplerState objSamplerState : SAMPLER;

struct PsIn
{
	float4 pos : SV_POSITION;
	float3 pos_w : WORLDPOSITION;
};

float4 main(PsIn pin) : SV_TARGET
{
	//return float4(cube_map.SampleLevel(objSamplerState,pin.pos_w,3.f).xyz,1.f);
	return float4(cube_map.Sample(objSamplerState,pin.pos_w).xyz,1.f);
}