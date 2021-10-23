struct VSOut
{
	float3 color : Color;
	float4 pos : SV_Position;
};

cbuffer CBuf
{
	row_major matrix MVPMartrix;
};


VSOut main(float3 pos : POSITION, float3 color : COLOR) //: SV_POSITION
{
	VSOut respos;
	respos.pos = mul(float4(pos, 1.f), MVPMartrix);
	respos.color = color;
	return respos;
}