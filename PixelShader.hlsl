Texture2D textureSampler : register(t0);
SamplerState samplerState : register(s0);

struct VS_Output
{
    float4 pos : SV_POSITION;
    float2 textureCoordinate : TEXTURECOORDINATE0;
};

float4 main(VS_Output input) : SV_TARGET
{
    float4 texColor = textureSampler.Sample(samplerState, input.textureCoordinate);
    return texColor.zyxw;
}
