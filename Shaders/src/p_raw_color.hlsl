cbuffer LightShader
{
	float3 color : COLOR;
	float intensity : ALPHA;
}

float4 main() : SV_TARGET
{
	return intensity * float4(color, 1.f);
}