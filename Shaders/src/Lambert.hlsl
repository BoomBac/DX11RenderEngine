struct PSInput
{
	float4 pos : SV_Position;
	float3 posW : WORLD_POSTION;
	float3 cameraPos : CAMERA_POSTION;
	float2 uv : Texcoord;
	float3 normal : NORMAL;
};
cbuffer LightBuffer : register(b0)
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

cbuffer LightMatrix : register(b1)
{
	row_major matrix light_view_projection;
}

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER;

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
	float3 pix_color = objTexture.Sample(objSamplerState, input.uv);
	//float4 unlit = (input.uv.r + input.uv.g)/2 * float4(0.5f, 0.5f, 0.5f, 1.f);
	float3 lit;
	if (light_type==0.f)
	{
		lit = light_color * max(0.f, dot(normalize(input.normal), -normalize(input.posW - light_pos))) * clamp(1 - pow(distance(input.posW, light_pos) / affect_radius, 4.f), 0.f, 2.f);
	}
	else if (light_type==1.f)
	{
		lit = max(0.f, dot(normalize(input.normal), -light_dir)) * light_color;

	}
	else if (light_type==2.f)
	{
		lit = light_color * pow(clamp((dot(normalize(light_pos - input.posW), light_dir) - cos(outer_angle)) / (cos(inner_angle_) - cos(outer_angle)),0.f,1.f), 2.f)
              * clamp(1 - pow(distance(input.posW, light_pos) / affect_radius, 4.f), 0.f, 2.f)  ;
	}
	//将当前点映射到lightCoordinate
	float4 pos_light = mul(float4(input.posW,1.f), light_view_projection);
	//计算当前点在shadowmap中的uv
	float2 shadow_uv;
	shadow_uv.x = (pos_light.x / pos_light.w)*0.5f + 0.5f;
	shadow_uv.y = (pos_light.y / pos_light.w)*-0.5f + 0.5f;
	//计算当前点在lightCoordinate中的深度
	float dep_camera = pos_light.z / pos_light.w;
	float bias = max(0.0000004f * (1.f - dot(input.normal, normalize(light_pos - input.posW))), 0.0000003f);
	if (dep_camera - bias > objTexture.Sample(objSamplerState, shadow_uv).x)
		return float4(0.1f, 0.1f, 0.1f, 1.f);
	else
		return float4(light_intensity * lit, 1.f) + float4(0.3,0.3,0.3,1.f);
}