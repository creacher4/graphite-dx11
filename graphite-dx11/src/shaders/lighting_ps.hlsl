Texture2D Albedo : register(t0);
Texture2D Normals : register(t1);
SamplerState samp : register(s0);

struct PS_INPUT
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_Target
{
    float3 albedo = Albedo.Sample(samp, input.texCoord).rgb;
    float3 normal = normalize(Normals.Sample(samp, input.texCoord).xyz);
    float3 LightDir = normalize(float3(0.577, -0.577, 1.0));
    float diff = saturate(dot(normal, -LightDir));
    return float4(albedo * diff, 1.0);
}