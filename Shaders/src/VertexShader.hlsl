struct VSOut
{
	float4 pos : SV_Position;
	float2 uv : Texcoord;
};

cbuffer CBuf
{
	row_major matrix MVPMartrix;
};


VSOut main(float3 pos : Position, float2 uv : Texcoord, float3 normal : Normal) //: SV_POSITION
{
	VSOut respos;
	respos.pos = mul(float4(pos, 1.f), MVPMartrix);
	respos.uv = uv;
	return respos;
}