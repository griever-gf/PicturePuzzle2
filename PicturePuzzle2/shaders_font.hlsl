Texture2D ObjTexture;
SamplerState ObjSamplerState;

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

PixelInputType FONT_VS(VertexInputType input)
{
	PixelInputType output;

	output.position = input.position;
    output.tex = input.tex;
    
    return output;
}

float4 FONT_PS(PixelInputType input) :SV_TARGET
{

	float4 color = ObjTexture.Sample( ObjSamplerState, input.tex);

	if(color.a < 0.5)
      discard;

	return color;
}