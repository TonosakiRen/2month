

struct Param {
	float param;
};
ConstantBuffer<Param> t  : register(b0);

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float4> resultTex : register(t0);
Texture2D<float4> tex : register(t1);
SamplerState smp : register(s0);

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;
	float32_t4 result = resultTex.Sample(smp, input.uv);
	float32_t4 addTex = tex.Sample(smp, input.uv);
	addTex = float32_t4( 0.0f,0.0f,0.0f,1.0f );
	output.color = lerp(result, addTex, t.param);
	return output.color;
}