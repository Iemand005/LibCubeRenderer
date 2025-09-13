// Texture and Sampler declarations
Texture2D textureSampler : register(t0);
SamplerState samplerState : register(s0); // Sampler state

// The structure coming from the vertex shader
struct VS_Output
{
    float4 pos : SV_POSITION;
    float2 textureCoordinate : TEXTURECOORDINATE0;
};

// The main pixel shader function
float4 main(VS_Output input) : SV_TARGET
{
    float4 texColor = textureSampler.Sample(samplerState, input.textureCoordinate);
    return texColor;
}
