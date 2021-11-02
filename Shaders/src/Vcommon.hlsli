//common const value

static const float g_PI = 3.14159265359f;

cbuffer cbPerFrame : register(b0)
{
	row_major matrix gView;
	row_major matrix gProj;
	row_major matrix gViewProj;
	float3 gCamreaPos;
	float padding;
};

