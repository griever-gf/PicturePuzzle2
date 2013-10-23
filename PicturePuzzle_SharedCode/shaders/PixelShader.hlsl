//float4 main() : SV_TARGET
//{
//    return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}
Texture2D shaderTexture;
SamplerState SampleType;
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
	//textureColor = (0.0, 0.2, 0.4, 1.0);
    textureColor = shaderTexture.Sample(SampleType, input.tex);
	if(textureColor.a < 0.05) //"alpha"-channel
      discard;
    return textureColor;
}