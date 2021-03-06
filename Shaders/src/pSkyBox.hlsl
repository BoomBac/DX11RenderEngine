Texture2D sphere_map : register(t0);
SamplerState objSamplerState : SAMPLER;


struct PsIn
{
	float4 pos : SV_POSITION;
	float3 pos_w : WORLDPOSITION;
};
static const float2 invAtan = float2(0.1591, 0.3183);

float2 SampleSphericalMap(float3 v)
{
	float2 uv = float2(atan2(v.x, v.z), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

float4 main(PsIn pin) : SV_TARGET
{
	float2 uv =  SampleSphericalMap(normalize(pin.pos_w));
	return float4(sphere_map.Sample(objSamplerState,uv).xyz,1.f);
}