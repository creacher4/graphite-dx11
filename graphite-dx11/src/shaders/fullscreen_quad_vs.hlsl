struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

VS_OUTPUT main(uint id : SV_VertexID)
{
    float2 pos[3] =
    {
        float2(-1, -1),
        float2(-1, 3),
        float2(3, -1)
    };

    float2 uvs[3] =
    {
        float2(0, 0),
        float2(0, 2),
        float2(2, 0)
    };

    VS_OUTPUT output;
    output.position = float4(pos[id], 0, 1);
    output.texCoord = uvs[id];
    return output;
}