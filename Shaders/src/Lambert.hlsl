struct PSInput
{
	float4 pos : SV_Position;
	float3 posW : WORLD_POSTION;
	float3 cameraPos : CAMERA_POSTION;
	float2 uv : Texcoord;
	float3 normal : NORMAL;
};
cbuffer LightBuffer
{
	float3 light_pos;
	float light_intensity;
	float3 light_dir;
	float padding;
	float4 light_color;
	float affect_radius;
	float inner_angle_;
	float outer_angle;
	float light_type;
};
float3 Gooch(float3 dir_camera,float3 dir_light,float3 normal)
{
	float t = (dot(normal, dir_light) + 1) / 2.f;
	float3 r = 2 * dot(normal, dir_light) * dir_light - dir_light;
	float s = clamp(100 * dot(r, dir_camera) - 97, 0.f, 1.f);
	return s * float3(1.f, 1.f, 1.f) + (1.f - s) * (t * float3(0.f, 0.f, 0.55f) + (1 - t) * float3(0.3f, 0.3f, 0.f));
}
float4 main(PSInput input) : SV_TARGET
{
	//float4 unlit = float4(Gooch(input.posW - input.cameraPos, input.posW - light_pos, input.normal), 1.f);
	float3 lit;
	if (light_type==0.f)
	{
		lit = clamp(1 - pow(distance(input.posW, light_pos) / affect_radius, 4.f), 0.f, 2.f);
	}
	else if (light_type==1.f)
	{
		lit = max(0.f, dot(input.normal, -light_dir)) * light_color;

	}
	else if (light_type==2.f)
	{
		lit = light_color * pow(clamp((dot(normalize(light_pos - input.posW), light_dir) - cos(outer_angle)) / (cos(inner_angle_) - cos(outer_angle)),0.f,1.f), 2.f)
              * clamp(1 - pow(distance(input.posW, light_pos) / affect_radius, 4.f), 0.f, 2.f)  ;
	}
		return float4(light_intensity * lit, 1.f) + float4(0.5f, 0.5f, 0.5f, 1.f);
	}