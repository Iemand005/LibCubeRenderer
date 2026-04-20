struct VS_Input
{
    float3 position : POSITION;
    float2 textureCoordinate : TEXTURECOORDINATE0; // Use float instead of half
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float2 textureCoordinate : TEXTURECOORDINATE0; // Use float instead of half
};

cbuffer ConstantBuffer : register(b0)
{
    matrix transform;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    output.position = mul(float4(input.position, 1.0f), transform);
    output.textureCoordinate = input.textureCoordinate;
    return output;
}
