#include "Vcommon.hlsli"

struct VSOut
{
	float4 pos : SV_Position;
	float3 posW : WORLD_POSTION;
	float3 cameraPos : CAMERA_POSTION;
	float2 uv : Texcoord;
	float3 normal : NORMAL;
	matrix<float, 3,3> btn : BTN;
};

cbuffer CBuf : register(b1)
{
	row_major matrix WorldMatrix;
};

VSOut main(float3 pos : POSITION, float2 uv : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT) //: SV_POSITION
{
	VSOut respos;
	//matrix MVPMartrix = mul(gViewProj, WorldMatrix);
	row_major matrix MVPMartrix = mul(WorldMatrix, gViewProj);
	//matrix MVPMartrix = WorldMatrix;
	respos.pos = mul(float4(pos, 1.f), MVPMartrix);
	respos.uv = uv;
	respos.cameraPos = gCamreaPos;
	respos.posW = mul(float4(pos, 1.f), WorldMatrix);
	respos.normal = normalize(mul(normal, WorldMatrix));
	float3 T = normalize((mul(tangent, WorldMatrix)));
	float3 B = normalize(cross(respos.normal, T));
	respos.btn = matrix<float,3,3>(T, B, respos.normal);
	return respos;
}