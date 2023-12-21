Texture2D<float4> src_ : register(t0);

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t4 texcoord[8] : TEXCOORD0;
};

VSOutput main(float32_t4 pos : POSITION, float32_t2 uv : TEXCOORD) {
	VSOutput output;

	float32_t2 textureSize;
	float level;
	src_.GetDimensions(0, textureSize.x, textureSize.y, level);

	float32_t2 base = uv;

	float32_t2 texelSize = float32_t2(0.0f, 1.0f / textureSize.y);

	output.svpos = float32_t4(lerp(float32_t2(-1.0f, 1.0f), float32_t2(1.0f, -1.0f), base), 0.0f, 1.0f);

	float32_t2 offset = texelSize;
	float32_t4 basebase = float32_t4(base, base);

	for (uint i = 0; i < 8; ++i) {
		output.texcoord[i].xy = offset;
		output.texcoord[i].zw = output.texcoord[i].xy * -1.0f;
		output.texcoord[i] += basebase;
		offset = texelSize * 2.0f;
	}

	return output;
}