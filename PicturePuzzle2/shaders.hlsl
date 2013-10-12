struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;

    output.position = input.position;
    output.color = input.color;
    
    return output;
}

float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
    return input.color;
}