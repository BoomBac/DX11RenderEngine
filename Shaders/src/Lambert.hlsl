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
float PCF(float depth_camera,float2 shadow_uv,int quality,float bias)
{
	float sum = 0.f;
	quality = min(quality, 25);
	for (int i = 0; i < quality; ++i)
	{
		for (int j = 0; j < quality; ++j)
		{
			float2 uv = float2(shadow_uv.x + (j - quality / 2) * 0.00125, shadow_uv.y + (i - quality / 2) * 0.00167);
			sum += depth_camera - bias < objTexture.Sample(objSamplerState, uv).x;
		}
	}
	return sum /= pow(quality, 2);
}

float PCSS(float depth_camera, float2 shadow_uv,float bias)
{
	//if (depth_camera - bias < objTexture.Sample(objSamplerState, shadow_uv).x)
	//	return 1.f;
	//1.calucate avg block-depth
	float block_dep_avg = 0.f;
	int block_count = 0;
	float2 uv;
	float block_dep;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			uv = float2(shadow_uv.x + (j - 2) * 0.00125, shadow_uv.y + (i - 2) * 0.00167);
			block_dep = objTexture.Sample(objSamplerState, uv).x;
			if (depth_camera - bias > block_dep);
			{
				block_dep_avg += block_dep;
				++block_count;
			}
		}
	}
	//if (block_count > 0)
	//	block_dep_avg /= block_count;
	//else
	//	return 1.f;
	//2.caculate Penumbra
	//float quality = abs(depth_camera - block_dep_avg) / block_dep_avg * 200.f;
	//float radius = quality * light_intensity;
	block_dep_avg = (block_dep_avg / block_count - 0.9f) * 10;
	return block_dep_avg;

}

float4 main(PSInput input):SV_TARGET
{
//float4 unlit = float4(Gooch(input.posW - input.cameraPos, input.posW - light_pos, input.normal), 1.f);
	float3 pix_color = objTexture.Sample(objSamplerState, input.uv);
//float4 unlit = (input.uv.r + input.uv.g)/2 * float4(0.5f, 0.5f, 0.5f, 1.f);
	float3 lit;
	if (light_type == 0.f)
	{
		lit = light_color * max(0.f, dot(normalize(input.normal), -normalize(input.posW - light_pos))) * clamp(1 - pow(distance(input.posW, light_pos) / affect_radius, 4.f), 0.f, 2.f);
	}
	else if (light_type == 1.f)
	{
		lit = max(0.f, dot(normalize(input.normal), -light_dir)) * light_color;

	}
	else if (light_type == 2.f)
	{
		lit = light_color * pow(clamp((dot(normalize(light_pos - input.posW), light_dir) - cos(outer_angle)) / (cos(inner_angle_) - cos(outer_angle)), 0.f, 1.f), 2.f)
            * clamp(1 - pow(distance(input.posW, light_pos) / affect_radius, 4.f), 0.f, 2.f);
	}
//将当前点映射到lightCoordinate
	float4 pos_light = mul(float4(input.posW, 1.f), light_view_projection);
//计算当前点在shadowmap中的uv
	float2 shadow_uv;
	shadow_uv.x = (pos_light.x / pos_light.w) * 0.5f + 0.5f;
	shadow_uv.y = (pos_light.y / pos_light.w) * -0.5f + 0.5f;
//计算当前点在lightCoordinate中的深度
	float dep_camera = pos_light.z / pos_light.w;
	float bias = max(0.0000006f * (1.f - dot(input.normal, normalize(light_pos - input.posW))), 0.0000005f);

	//return PCF(dep_camera, shadow_uv,5, 0.000004f) * float4(light_intensity * lit, 1.f); //+ float4(0.3, 0.3, 0.3, 1.f);
	//return PCF(dep_camera, shadow_uv, 100, 0.000004f) * float4(1.f, 1.f, 1.f, 1.f);
	//float radius = PCSS(dep_camera, shadow_uv,bias);
	return PCSS(dep_camera, shadow_uv, bias) * float4(1.f,1.f,1.f,1.f); // * float4(light_intensity * lit, 1.f); //+ float4(0.3, 0.3, 0.3, 1.f);
}