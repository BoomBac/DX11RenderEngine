float3 GetNormal(in Texture2D map, float2 uv, SamplerState state,float3x3 btn)
{
	float3 N = map.Sample(state,uv).xyz *2.f - 1.f;
	N = normalize(mul(N,btn));
	return N;
}
