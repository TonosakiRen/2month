
struct Param {
	float32_t3 edgeColor;
};
ConstantBuffer<Param> param_  : register(b0);

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2 texCenter : TEXCOORD0;
	float32_t2 texLT : TEXCOORD1;
	float32_t2 texMT : TEXCOORD2;
	float32_t2 texRT : TEXCOORD3;
	float32_t2 texLM : TEXCOORD4;
	float32_t2 texRM : TEXCOORD5;
	float32_t2 texLB : TEXCOORD6;
	float32_t2 texMB : TEXCOORD7;
	float32_t2 texRB : TEXCOORD8;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float4> shadowTex : register(t0);

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	PixelShaderOutput output;


	float32_t2 texSize;
	//テクスチャーのサイズ
	shadowTex.GetDimensions(texSize.x, texSize.y);

	float32_t3 color;
	//周囲のピクセルの法線の値の平均を計算する
	color = shadowTex.Sample(smp, input.texCenter).xyz * -8.0f;
	color += shadowTex.Sample(smp, input.texLT.xy).xyz;
	color += shadowTex.Sample(smp, input.texMT.xy).xyz;
	color += shadowTex.Sample(smp, input.texRT.xy).xyz;
	color += shadowTex.Sample(smp, input.texLM.xy).xyz;
	color += shadowTex.Sample(smp, input.texRM.xy).xyz;
	color += shadowTex.Sample(smp, input.texLB.xy).xyz;
	color += shadowTex.Sample(smp, input.texMB.xy).xyz;
	color += shadowTex.Sample(smp, input.texRB.xy).xyz;


	//法線の計算結果、あるいは深度値の計算結果が一定以上ならエッジとみなす。
	if (color.x >= 0.08f) {
		output.color.xyz = param_.edgeColor;
		output.color.w = 1.0f;
		if (shadowTex[uint32_t2(input.texCenter.x * texSize.x - 1, input.texCenter.y * texSize.y)].y == 2) {
			output.color.xyz = float32_t3(0.5f, 0.0f, 0.5f);
		}else
		if (shadowTex[uint32_t2(input.texCenter.x * texSize.x + 1, input.texCenter.y * texSize.y)].y == 2) {
			output.color.xyz = float32_t3(0.5f, 0.0f, 0.5f);
		}else
		if (shadowTex[uint32_t2(input.texCenter.x * texSize.x , input.texCenter.y * texSize.y - 1)].y == 2) {
			output.color.xyz = float32_t3(0.5f, 0.0f, 0.5f);
		}else
		if (shadowTex[uint32_t2(input.texCenter.x * texSize.x , input.texCenter.y * texSize.y + 1)].y == 2) {
			output.color.xyz = float32_t3(0.5f, 0.0f, 0.5f);
		}else
		if (shadowTex[uint32_t2(input.texCenter.x * texSize.x + 1, input.texCenter.y * texSize.y + 1)].y == 2) {
			output.color.xyz = float32_t3(0.5f, 0.0f, 0.5f);
		}else
		if (shadowTex[uint32_t2(input.texCenter.x * texSize.x - 1, input.texCenter.y * texSize.y - 1)].y == 2) {
			output.color.xyz = float32_t3(0.5f, 0.0f, 0.5f);
		}else
		if (shadowTex[uint32_t2(input.texCenter.x * texSize.x + 1, input.texCenter.y * texSize.y - 1)].y == 2) {
			output.color.xyz = float32_t3(0.5f, 0.0f, 0.5f);
		}else
		if (shadowTex[uint32_t2(input.texCenter.x * texSize.x - 1 , input.texCenter.y * texSize.y + 1)].y == 2) {
			output.color.xyz = float32_t3(0.5f, 0.0f, 0.5f);
		}
		
	}
	else {
		output.color = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	return output.color;
}