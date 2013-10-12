Texture2D shaderTexture;
SamplerState SampleType;

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;

	output.position = input.position;
    output.tex = input.tex;
    
    return output;
}

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
	//textureColor = (0.5, 0.5, 0.5, 0.5);
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    return textureColor;
}