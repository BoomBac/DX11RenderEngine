cbuffer CBuf
{
	row_major matrix MVPMartrix;
	row_major matrix WorldMartrix;
	float3 CameraPos;
};
struct VsOut
{
	float4 pos : SV_POSITION;
	float3 pos_w : WORLDPOSITION;
};
VsOut main(float3 pos : POSITION)
{
	VsOut ret;
	ret.pos = mul(float4(pos,1.f),MVPMartrix);
	ret.pos.xyzw = ret.pos.xyww;
	ret.pos_w = pos;
	return ret;
}