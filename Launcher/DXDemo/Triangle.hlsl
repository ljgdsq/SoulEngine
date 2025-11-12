


float4 VSMain(float4 position:POSITION) : SV_POSITION
{
    return position;
}


float4 PSMain() : SV_TARGET
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f); // Red color
}