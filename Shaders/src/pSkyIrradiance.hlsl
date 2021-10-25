TextureCube cube_map : register(t0);
SamplerState objSamplerState : SAMPLER;

struct PsIn
{
    float4 pos : SV_POSITION;
    float3 pos_w : WORLDPOSITION;
};

static const float PI = 3.14159265359f;
static const float t_PI = 6.28318530718;
static const float h_PI = 1.570796326795;
static const float sampleDelta = 0.1;

float4 main(PsIn pin) : SV_TARGET
{
    float3 normal = normalize(pin.pos_w);
    float3 irradiance = float3(0.f,0.f,0.f);
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));
    float nrSamples = 0.0;
    [unroll(1000)]
    for (float phi = 0.0; phi < t_PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < h_PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
            irradiance += cube_map.Sample(objSamplerState, sampleVec).rgb * cos(theta)* sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    return float4(irradiance, 1.f);
}
