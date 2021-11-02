truct VSOut
{
	float4 pos : SV_POSITION;
	float3 posW : WORLD_POSTION;
	float3 cameraPos : CAMERA_POSTION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

cbuffer CBuf
{
	row_major matrix MVPMartrix;
	row_major matrix WorldMartrix;
	float3 CameraPos;
};


VSOut main(float3 pos : POSTION, float3 color : COLOR) //: SV_POSITION
{
	VSOut respos;
	respos.pos = mul(float4(pos, 1.f), MVPMartrix);
	respos.uv = pow(float2(color.r, color.b),2.f);
	respos.posW = mul(float4(pos, 1.f), WorldMartrix);
	respos.cameraPos = CameraPos;
	respos.normal = float3(0.f, 1.f, 0.f);
	return respos;
}