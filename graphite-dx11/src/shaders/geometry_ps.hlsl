struct PS_Input
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

float4 main(PS_Input input) : SV_Target
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}