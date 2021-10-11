struct VSOut
{
	float4 pos : SV_Position;
	float3 posW : WORLD_POSTION;
	float3 cameraPos : CAMERA_POSTION;
	float2 uv : Texcoord;
	float3 normal : NORMAL;
};

cbuffer CBuf
{
	row_major matrix MVPMartrix;
	row_major matrix WorldMartrix;
	float3 CameraPos;
};

VSOut main(float3 pos : Position, float2 uv : Texcoord, float3 normal : Normal) //: SV_POSITION
{
	VSOut respos;
	respos.pos = mul(float4(pos, 1.f), MVPMartrix);
	respos.uv = uv;
	respos.cameraPos = CameraPos;
	respos.posW = mul(float4(pos, 1.f), WorldMartrix);
	respos.normal = normalize(mul(float4(normal, 0.f), WorldMartrix));
	return respos;
}