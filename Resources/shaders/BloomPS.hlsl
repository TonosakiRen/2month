#define NUM_TEXTURES 4

Texture2D<float4> blurTexture0_ : register(t0);
Texture2D<float4> blurTexture1_ : register(t1);
Texture2D<float4> blurTexture2_ : register(t2);
Texture2D<float4> blurTexture3_ : register(t3);
SamplerState sampler_ : register(s0);

struct Param {
	float intensity;
};
ConstantBuffer<Param> param_ : register(b0);

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;

    float32_t4 bloom = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
    bloom += blurTexture0_.Sample(sampler_, input.uv);
    bloom += blurTexture1_.Sample(sampler_, input.uv);
    bloom += blurTexture2_.Sample(sampler_, input.uv);
    bloom += blurTexture3_.Sample(sampler_, input.uv);
    bloom /= NUM_TEXTURES;
    bloom.a = 1.0f;


    output.color = bloom;

    return output.color;
}