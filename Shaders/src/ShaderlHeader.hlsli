//common const value

static const float g_PI = 3.14159265359f;

cbuffer cbPerFrame : register(b0)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
}

