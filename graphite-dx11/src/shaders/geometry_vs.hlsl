struct VS_Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    output.position = float4(input.position, 1.0f);
    output.normal = input.normal;
    return output;
}