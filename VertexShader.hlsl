cbuffer CBuf
{
	 matrix transform;
};

float4 main(float2 pos : POSITION) : SV_POSITION
{
	return mul(transform,float4(pos.x, pos.y, 0.f, 1.f));
}