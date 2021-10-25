static const float PI = 3.14159265359;

float3 GetNormal(in Texture2D map, float2 uv, SamplerState state,float3x3 btn)
{
	float3 N = map.Sample(state,uv).xyz *2.f - 1.f;
	N = normalize(mul(N,btn));
	return N;
}
float4 ReflectFormEnv(TextureCube environment,SamplerState state,float3 eye_pos,float3 p_pos,float3 normal)
{
	float3 I = normalize(p_pos - eye_pos);
	float3 R = reflect(I,normal);
	return float4(environment.Sample(state,R).rgb,1.f);
}
float4 RefractFromEnv(TextureCube environment, SamplerState state, float3 eye_pos, float3 p_pos, float3 normal,float ratio)
{
	float3 I = normalize(p_pos - eye_pos);
	float3 R = refract(I,normal,ratio);
	return float4(environment.Sample(state,R).rgb,1.f);
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