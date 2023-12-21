Texture2D<float4> src_ : register(t0);
SamplerState sampler_ : register(s0);

struct Param {
    float32_t threshold;
    float32_t knee;
};
ConstantBuffer<Param> param_ : register(b0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

float32_t Knee(float32_t x, float32_t n, float32_t k) {
    float32_t s = k / n * x;
    float32_t e = (1.0f - k) / (1.0f - n) * (x - 1) + 1;
    return lerp(s, e, step(n, x));
}

struct VSOutput {
    float32_t4  svpos : SV_POSITION;
    float32_t2  uv : TEXCOORD;
};

float32_t4 main(VSOutput input) : SV_TARGET
{
    PixelShaderOutput output;

    output.color = src_.Sample(sampler_, input.uv);

    float32_t luminance = dot(output.color.xyz, float32_t3(0.2125f, 0.7154f, 0.0721f));
    //clip(luminance - param_.threshold);
    output.color.xyz *= Knee(luminance, param_.threshold, param_.knee);

    return output.color;
}