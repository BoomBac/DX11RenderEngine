struct VSOut
{
	float4 pos : SV_Position;
	float3 posW : WORLD_POSTION;
	float3 cameraPos : CAMERA_POSTION;
	float2 uv : Texcoord;
	float3 normal : NORMAL;
	matrix<float, 3,3> btn : BTN;
};

cbuffer CBuf
{
	row_major matrix MVPMartrix;
	row_major matrix WorldMartrix;
	float3 CameraPos;
};

VSOut main(float3 pos : POSTION, float2 uv : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT) //: SV_POSITION
{
	VSOut respos;
	respos.pos = mul(float4(pos, 1.f), MVPMartrix);
	respos.uv = uv;
	respos.cameraPos = CameraPos;
	respos.posW = mul(float4(pos, 1.f), WorldMartrix);
	respos.normal = normalize(mul(normal, WorldMartrix));
	float3 T = normalize((mul(tangent,WorldMartrix)));
	float3 B = normalize(cross(respos.normal, T));
	respos.btn = matrix<float,3,3>(T, B, respos.normal);
	return respos;
}