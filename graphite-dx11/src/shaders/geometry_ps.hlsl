struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

struct PS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    output.albedo = float4(1.0f, 0.0f, 0.0f, 1.0f);
    output.normal = float4(normalize(input.normal), 1.0f);
    return output;




    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
}