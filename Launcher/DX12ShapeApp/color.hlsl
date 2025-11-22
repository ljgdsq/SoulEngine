cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
};


void VS(float3 iPos:POSITION,float4 iColor:COLOR,out float4 oPosH:SV_POSITION,out float4 oColor:COLOR)
{

        oPosH = mul(float4(iPos, 1.0f), gWorldViewProj);
    // oPosH = mul(gWorldViewProj, float4(iPos, 1.0f));
    oColor = iColor;
}

float4 PS(float4 posH:SV_POSITION,float4 color:COLOR) : SV_Target
{
    return color;
}