Texture2D<float4> edgeTexture_ : register(t0);
SamplerState sampler_ : register(s0);

struct VSOutput {
    float32_t4  svpos : SV_POSITION;
    float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

float32_t4 main(VSOutput input) : SV_TARGET{
    PixelShaderOutput output;

    output.color = edgeTexture_.Sample(sampler_, input.uv);

    return output.color;
}