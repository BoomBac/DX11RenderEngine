cbuffer CBuf
{
	row_major matrix MVPMartrix;
	row_major matrix WorldMartrix;
	float3 CameraPos;
};
struct VsOut
{
	float4 pos : SV_POSITION;
	float3 pos_w : WORLDPOSTION;
};
VsOut main(float3 pos : POSTION)
{
	VsOut ret;
	ret.pos = mul(float4(pos,1.f),MVPMartrix);
	ret.pos_w = normalize(pos);
	return ret;
}