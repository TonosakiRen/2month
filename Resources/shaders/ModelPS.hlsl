
Texture2D<float4> tex : register(t0);
Texture2D<float4> shadowMap : register(t1);
SamplerState smp : register(s0);

struct Material {
	float32_t4 materialColor; //Materialの色
	float32_t4x4 uvTransfrom;//uvtransform
	int32_t enableLighting; //lighitngするか
};
ConstantBuffer<Material> gMaterial  : register(b2);

struct VSOutput {
	float32_t4 pos : SV_POSITION; // システム用頂点座標
	float32_t3 normal : NORMAL;     // 法線ベクトル
	float32_t2 uv : TEXCOORD;       // uv値
	float32_t4 worldPosition : POSITION0;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 normal : SV_TARGET1;
	float32_t4 enableLighting : SV_TARGET2;
};

PixelShaderOutput main(VSOutput input) {

	PixelShaderOutput output;

	float32_t3 normal = normalize(input.normal);
	output.enableLighting = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
	output.enableLighting.x = gMaterial.enableLighting;


	output.color = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);

	// マテリアル
	float32_t4 tranformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gMaterial.uvTransfrom);
	float32_t4 texColor = tex.Sample(smp, tranformedUV.xy);
	output.color.xyz += gMaterial.materialColor.xyz * texColor.xyz;

	output.normal.xyz = (normal.xyz + 1.0f) * 0.5f;
	output.normal.w = 1.0f;

	return output;
}