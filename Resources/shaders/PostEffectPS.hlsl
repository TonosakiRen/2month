

struct Param {
	float constant;
};
ConstantBuffer<Param> param_  : register(b0);

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;
	output.color = tex.Sample(smp, input.uv);
	return output.color;
}