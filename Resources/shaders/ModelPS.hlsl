struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

Texture2D<float4> tex : register(t0);
Texture2D<float4> shadowMap : register(t1);
SamplerState smp : register(s0);

struct Material {
	float32_t4 materialcolor; //Materialの色
	float32_t4x4 uvTransfrom;//uvtransform
	int32_t enableLighting; //lighitngするか
};
ConstantBuffer<Material> gMaterial  : register(b3);

struct VSOutput {
	float32_t4 pos : SV_POSITION; // システム用頂点座標
	float32_t3 normal : NORMAL;     // 法線ベクトル
	float32_t2 uv : TEXCOORD;       // uv値
	float32_t4 worldPosition : POSITION0;
	float32_t4 lightViewPosition : POSITION1;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 normal : SV_TARGET1;
};

PixelShaderOutput main(VSOutput input) {

	PixelShaderOutput output;

	float32_t3 normal = normalize(input.normal);

	output.color = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);

	// マテリアル
	float32_t4 tranformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gMaterial.uvTransfrom);
	float32_t4 texColor = tex.Sample(smp, tranformedUV.xy);
	output.color.xyz += gMaterial.materialcolor.xyz * texColor.xyz;

	//影
	float32_t2 shadowMapUV = input.lightViewPosition.xy / input.lightViewPosition.w;
	shadowMapUV *= float32_t2(0.5f, -0.5f);
	shadowMapUV += 0.5f;

	float32_t zInLVP = input.lightViewPosition.z / input.lightViewPosition.w;

	if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
		&& shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f
		) {
		float32_t zInShadowMap = shadowMap.Sample(smp, shadowMapUV).r;
		if (zInLVP - 0.00001 > zInShadowMap) {
			output.color.xyz *= 0.5f;
		}
	}

	output.normal.xyz = (normal.xyz + 1.0f) * 0.5f;
	output.normal.w = 1.0f;

	return output;
}