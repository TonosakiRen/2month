struct VSOutput {
	float32_t4 pos : SV_POSITION; // システム用頂点座標
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VSOutput input)
{

	PixelShaderOutput output;

	output.color = float32_t4(0.5f, 0.5f, 0.5f, 1.0f);

	return output;
}