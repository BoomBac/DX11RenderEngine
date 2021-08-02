cbuffer CBuf
{
	row_major matrix MVPMartrix;
};

float4 main(float3 pos : Position) : SV_POSITION
{
	float4 respos = float4(pos,1.f);
	respos = mul(respos, MVPMartrix);
	return respos;
}