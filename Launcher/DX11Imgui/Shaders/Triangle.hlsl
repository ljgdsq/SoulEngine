struct VertexIn
{
    float3 pos: POSITION;
    float4 color: COLOR;
};


struct VertexOut
{
    float4 pos: SV_POSITION;
    float4 color: COLOR;
};


cbuffer cbTransform : register(b0)
{
    matrix g_world;
    matrix g_view;
    matrix g_proj;
    float4 g_color;
    uint g_useCustomColor;
}


VertexOut VS(VertexIn vIn)
{
    VertexOut vOut;
    vOut.pos = mul(float4(vIn.pos, 1.0),g_world);
    vOut.pos = mul(vOut.pos, g_view);
    vOut.pos = mul(vOut.pos, g_proj);
    vOut.color = vIn.color;
    return vOut;
}

float4 PS(VertexOut vOut) : SV_Target
{
    return g_useCustomColor ? g_color : vOut.color;
}
