#include "PCSS.hlsli"
#include "Pcommon.hlsli"

struct PSInput
{
	float4 pos : SV_Position;
	float3 posW : WORLD_POSTION;
	float3 cameraPos : CAMERA_POSTION;
	float2 uv : Texcoord;
	float3 normal : NORMAL;
};
cbuffer LightMatrix : register(b1)
{
	row_major matrix light_view_projection;
	row_major matrix light_view;
	row_major matrix light_projection;
}
cbuffer ShadowProperty : register(b2)
{
	float     u_LightSize;
	float     u_LightBias;
	float     u_LightNear;
	float     u_LightFar;
};
static const int       u_Ortho = 0;
static const int       u_BlockerSearchSamples = 128;
static const int       u_PCFSamples = 128;
static const int       u_Visualization;
static const float     u_BlockerSearchScale = 1.f;

static const int width = 800;
static const int height = 600;



Texture2D objTexture : TEXTURE : register(t5);
Texture2D finalColor : TEXTURE : register(t10);
SamplerState objSamplerState : SAMPLER;

// Using similar triangles from the surface point to the area light
float search_region_radius_uv(float z)
{
    return u_BlockerSearchScale * u_LightSize * (z - u_LightNear) / z;
}
// Using similar triangles between the area light, the blocking plane and the surface point
float penumbra_radius_uv(float zReceiver, float zBlocker)
{
    if (u_Ortho == 1)
        return abs(zReceiver - zBlocker) / zBlocker;
    else
        return abs(zReceiver - zBlocker) * u_LightSize / zBlocker;
}

// ------------------------------------------------------------------

// Project UV size to the near plane of the light
float project_to_light_uv(float penumbra_radius, float z)
{
    return penumbra_radius * u_LightNear / z;
}

// ------------------------------------------------------------------

float z_clip_to_eye(float z)
{
    if (u_Ortho == 1)
        return u_LightNear + (u_LightFar - u_LightNear) * z;
    else
        return u_LightFar * u_LightNear / (u_LightFar - z * (u_LightFar - u_LightNear));
}
// Returns average blocker depth in the search region, as well as the number of found blockers.
// Blockers are defined as shadow-map samples between the surface point and the light.
void find_blocker(out float accum_blocker_depth,
                  out float num_blockers,
                  out float max_blockers,
                  float2      uv,
                  float     z0,
                  float     bias,
                  float     search_region_radius_uv)
{
    accum_blocker_depth = 0.0;
    num_blockers        = 0.0;
    max_blockers        = float(u_BlockerSearchSamples);
    float biased_depth  = z0 - bias;
	[unroll(128)]
    for (int i = 0; i < u_BlockerSearchSamples; ++i)
    {
        float2 offset;

        if (u_BlockerSearchSamples == 25)
            offset = Postion25[i];
        else if (u_BlockerSearchSamples == 32)
            offset = Postion32[i];
        else if (u_BlockerSearchSamples == 64)
            offset = Postion64[i];
        else if (u_BlockerSearchSamples == 100)
            offset = Postion100[i];
        else if (u_BlockerSearchSamples == 128)
            offset = Postion128[i];

        offset *= search_region_radius_uv;
        float shadow_map_depth = objTexture.Sample(objSamplerState, uv+offset).x;

        if (shadow_map_depth < biased_depth)
        {
            accum_blocker_depth += shadow_map_depth;
            ++num_blockers;
        }
    }
}

float pcf_poisson_filter(float2 uv, float z0, float bias, float filter_radius_uv)
{
    float sum = 0.0;
	[unroll(128)]
    for (int i = 0; i < u_PCFSamples; ++i)
    {
        float2 offset;

        if (u_PCFSamples == 25)
            offset = Postion25[i];
        else if (u_PCFSamples == 32)
            offset = Postion32[i];
        else if (u_PCFSamples == 64)
            offset = Postion64[i];
        else if (u_PCFSamples == 100)
            offset = Postion100[i];
        else if (u_PCFSamples == 128)
            offset = Postion128[i];

        offset *= filter_radius_uv;
        float shadow_map_depth = objTexture.Sample(objSamplerState, uv+offset).x;
        sum += shadow_map_depth < (z0 - bias) ? 0.0 : 1.0;
    }

    return sum / float(u_PCFSamples);
}

float pcss_filter(float2 uv, float z, float bias, float z_vs, out float p_r, out float3 debug_color)
{
    // ------------------------
    // STEP 1: blocker search
    // ------------------------
    float accum_blocker_depth, num_blockers, max_blockers;
	float search_region_radius_uv_ = search_region_radius_uv(z_vs);
    find_blocker(accum_blocker_depth, num_blockers, max_blockers, uv, z, bias, search_region_radius_uv_);

    float avg_blocker_depth = accum_blocker_depth / num_blockers;

    // Early out if not in the penumbra
    if (num_blockers == 0.0)
    {
		debug_color = float3(0.f,0.f,0.f);
        return 1.0;
    }

    debug_color = num_blockers / max_blockers * float3(1.f,1.f,1.f);

    // ------------------------
    // STEP 2: penumbra size
    // ------------------------
	//z_vs当前像素在灯光摄像机坐标系z
    float avg_blocker_depth_vs = z_clip_to_eye(avg_blocker_depth);
    float penumbra_radius      = penumbra_radius_uv(z_vs, avg_blocker_depth_vs);
    float filter_radius        = project_to_light_uv(penumbra_radius, z_vs);
    p_r                        = filter_radius;

    // ------------------------
    // STEP 3: filtering
    // ------------------------
   // return ;
    //return num_blockers / 128.f;
	return pcf_poisson_filter(uv, z, bias, filter_radius);
}

float3 Gooch(float3 dir_camera,float3 dir_light,float3 normal)
{
	float t = (dot(normal, dir_light) + 1) / 2.f;
	float3 r = 2 * dot(normal, dir_light) * dir_light - dir_light;
	float s = clamp(100 * dot(r, dir_camera) - 97, 0.f, 1.f);
	return s * float3(1.f, 1.f, 1.f) + (1.f - s) * (t * float3(0.f, 0.f, 0.55f) + (1 - t) * float3(0.3f, 0.3f, 0.f));
}
//float PCF(float depth_camera,float2 shadow_uv,int quality,float bias)
//{
//	float sum = 0.f;
//	quality = min(quality, 25);
//	for (int i = 0; i < quality; ++i)
//	{
//		for (int j = 0; j < quality; ++j)
//		{
//			float2 uv = float2(shadow_uv.x + (j - quality / 2) * 1.f/(float)width, shadow_uv.y + (i - quality / 2) * 1.f/(float)height);
//			sum += depth_camera - bias < objTexture.Sample(objSamplerState, uv).x;
//		}
//	}
//	return sum /= pow(quality, 2);
//}
//
//float PCSS(float depth_camera, float pos_z, float2 shadow_uv, float bias)
//{
//	//if (depth_camera - bias < objTexture.Sample(objSamplerState, shadow_uv).x)
//	//	return 1.f;
//	//1.calucate avg block-depth
//	float block_dep_avg = 0.f;
//	int block_count = 0;
//	float2 uv;
//	float block_dep;
//	float min_dep = depth_camera;
//	for (int i = 0; i < 5; i++)
//	{
//		for (int j = 0; j < 5; j++)
//		{
//			uv = float2(shadow_uv.x + (j - 2) * 1.f / (float)width, shadow_uv.y + (i - 2) * 1.f / (float)height);
//			block_dep = objTexture.Sample(objSamplerState, uv).x;
//			if (depth_camera - bias > block_dep)
//			{
//				min_dep = min(min_dep, block_dep);
//				block_dep_avg += block_dep;
//				++block_count;
//			}
//		}
//	}
//	if (block_count == 0)
//		return 1.f;
//	//if (block_count <= 23)
//	//	return PCF(depth_camera, shadow_uv, 5, bias);
//	//2.caculate Penumbra
//	block_dep_avg /= block_count;
//	//depth map to z
//	float distance = g_kLightFar * g_kLightNear / (g_kLightFar - block_dep_avg * (g_kLightFar - g_kLightNear));
//	float min_distance = g_kLightFar * g_kLightNear / (g_kLightFar - min_dep * (g_kLightFar - g_kLightNear));
//	//similar tranangle to calculate penumbra
//	float penumbra = abs(pos_z - distance) / distance;
//	//penumbra to calculate radius
//	float radius = penumbra * 20.f * g_kLightNear / pos_z;
//	int quality = radius/ 0.00167f + min_dep;
//	//return (float)quality / 25.f + 0.00001f * min_distance;
//	return PCF(depth_camera, shadow_uv, quality, bias);
//}

float shadow_occlussion(float3 p,out float p_r,float bias,out float3 debug_color)
{
    // Transform frag position into Light-space.
    float4 light_space_pos = mul(float4(p, 1.f), light_view_projection);
	//	light_view_projection * float4(p, 1.f);

    float3 proj_coords = light_space_pos.xyz / light_space_pos.w;
    // transform to [0,1] range
    proj_coords.x = proj_coords.x * 0.5 + 0.5;
	proj_coords.y = proj_coords.y * -0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    // check whether current frag pos is in shadow
	debug_color = float3(1.f, 1.f, 1.f);
	float4 pos_vs = mul(float4(p, 1.f), light_view);//light_view * float4(p, 1.0);
    pos_vs.xyz /= pos_vs.w;

    if (proj_coords.x > 1.0 || proj_coords.y > 1.0 || proj_coords.z > 1.0 || proj_coords.x < 0.0 || proj_coords.y < 0.0 || proj_coords.z < 0.0)
        return 1.0;
	//return pcf_poisson_filter(proj_coords.xy, current_depth, bias, 0.0005f);
    return pcss_filter(proj_coords.xy, current_depth, bias, pos_vs.z, p_r, debug_color);
}


float4 main(PSInput input):SV_TARGET
{
	//float3 pix_color = objTexture.Sample(objSamplerState, input.uv);
	//float3 lit;
	//if (light_type == 0.f)
	//{
	//	lit = light_color * max(0.f, dot(normalize(input.normal), -normalize(input.posW - light_pos))) * clamp(1 - pow(distance(input.posW, light_pos) / affect_radius, 4.f), 0.f, 2.f);
	//}
	//else if (light_type == 1.f)
	//{
	//	lit = max(0.f, dot(normalize(input.normal), -light_dir)) * light_color;

	//}
	//else if (light_type == 2.f)
	//{
	//	lit = light_color * pow(clamp((dot(normalize(light_pos - input.posW), light_dir) - cos(outer_angle)) / (cos(inner_angle_) - cos(outer_angle)), 0.f, 1.f), 2.f)
 //           * clamp(1 - pow(distance(input.posW, light_pos) / affect_radius, 4.f), 0.f, 2.f);
	//}
	//map to lightCoordinate
	float4 pos_light = mul(float4(input.posW, 1.f), light_view_projection);
	float4 pos_light_w = mul(float4(input.posW, 1.f), light_view);
	//calculate shaodwmap uv
	pos_light.xyz /= pos_light.w;
	pos_light_w.xyz /= pos_light_w.w;
	float2 shadow_uv;
	shadow_uv.x = (pos_light.x) * 0.5f + 0.5f;
	shadow_uv.y = (pos_light.y) * -0.5f + 0.5f;
	//calculate bias
	float bias =max(u_LightBias * (1.f - dot(input.normal,-normalize(light_pos - input.posW))), 0.0001f);
	float r;
    float p_r;
    float3  num_blockers_color;
	float2 pix_pos = float2(input.pos.x / 800.f, input.pos.y / 600.f);
    float4 fcolor = finalColor.Sample(objSamplerState, pix_pos);
	return  0.4f * fcolor + fcolor * shadow_occlussion(input.posW, p_r, bias, num_blockers_color);
}