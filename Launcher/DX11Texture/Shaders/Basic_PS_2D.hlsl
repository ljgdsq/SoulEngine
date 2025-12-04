#include "Basic.hlsli"


float4 PS_2D(VertexPosHTex input) : SV_Target
{
    float4 texColor = gTex.Sample(gSamLinear, input.Tex);
    return texColor;
}