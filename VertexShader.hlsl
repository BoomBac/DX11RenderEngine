cbuffer CBuf
{
	row_major matrix World;
	row_major matrix View;
	row_major matrix Projection;
};

float4 main(float3 pos : Position) : SV_POSITION
{
	float4 respos = float4(pos,1.f);
	respos = mul(respos, World );
	respos = mul(respos,View);
	respos = mul(respos, Projection);
	return respos;
}