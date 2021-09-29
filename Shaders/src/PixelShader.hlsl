struct PSInput
{
	float4 pos : SV_Position;
	float2 uv : Texcoord;
};

Texture2D objTexture : TEXTURE : register(t0);

SamplerState objSamplerState : SAMPLER;

float4 main(PSInput input) : SV_TARGET
{
	float3 pix_color = objTexture.Sample(objSamplerState, input.uv);
	return float4(pix_color,1.f);
}