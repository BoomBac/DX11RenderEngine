#include "Vcommon.hlsli"

cbuffer CBuf : register(b1)
{
	row_major matrix WorldMatrix;
};
struct VsOut
{
	float4 pos : SV_POSITION;
	float3 pos_w : WORLDPOSITION;
};
VsOut main(float3 pos : POSITION)
{
	VsOut ret;
	row_major matrix MVPMartrix = mul(WorldMatrix, gViewProj);
	//matrix MVPMartrix = WorldMatrix;
	ret.pos = mul(float4(pos, 1.f), MVPMartrix);
	ret.pos.xyzw = ret.pos.xyww;
	ret.pos_w = pos;
	return ret;
}