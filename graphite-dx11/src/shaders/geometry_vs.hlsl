struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 1.0f);
    output.normal = input.normal;
    return output;
}