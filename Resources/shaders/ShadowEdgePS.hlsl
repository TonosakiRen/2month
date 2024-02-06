
struct Param {
	float32_t3 edgeColor;
};
ConstantBuffer<Param> param_  : register(b0);

struct CollisionPlayer {
	int32_t is;
};
ConstantBuffer<CollisionPlayer> collisionPlayer_  : register(b1);

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
	output.color.w = 0.0f;

	float32_t2 texSize;
	//テクスチャーのサイズ
	shadowTex.GetDimensions(texSize.x, texSize.y);

	float32_t3 color;

	float32_t4 centerData = shadowTex.Sample(smp, input.texCenter);
	float32_t4 LTData = shadowTex.Sample(smp, input.texLT.xy);
	float32_t4 MTData = shadowTex.Sample(smp, input.texMT.xy);
	float32_t4 RTData = shadowTex.Sample(smp, input.texRT.xy);
	float32_t4 LMData = shadowTex.Sample(smp, input.texLM.xy);
	float32_t4 RMData = shadowTex.Sample(smp, input.texRM.xy);
	float32_t4 LBData = shadowTex.Sample(smp, input.texLB.xy);
	float32_t4 MBData = shadowTex.Sample(smp, input.texMB.xy);
	float32_t4 RBData = shadowTex.Sample(smp, input.texRB.xy);

	//周囲のピクセルの法線の値の平均を計算する
	color = centerData.xyz * -8.0f;
	color += LTData.xyz;
	color += MTData.xyz;
	color += RTData.xyz;
	color += LMData.xyz;
	color += RMData.xyz;
	color += LBData.xyz;
	color += MBData.xyz;
	color += RBData.xyz;


	//法線の計算結果、あるいは深度値の計算結果が一定以上ならエッジとみなす。
	if (color.x >= 1.0f) {
		if (collisionPlayer_.is == 1) {
			output.color.xyz = param_.edgeColor;
			output.color.w = 1.0f;
		}

		int isEnemy = 0;
		int wData = 0;

		if (centerData.y == 2) {
			isEnemy = 1;
			wData = centerData.w;
		}else
		if (LTData.y == 2) {
			isEnemy = 1;
			wData = LTData.w;
		}else
		if (MTData.y == 2) {
			isEnemy = 1;
			wData = MTData.w;
		}else
		if (RTData.y == 2) {
			isEnemy = 1;
			wData = RTData.w;
		}else
		if (LMData.y == 2) {
			isEnemy = 1;
			wData = LMData.w;
		}else
		if (RMData.y == 2) {
			isEnemy = 1;
			wData = RMData.w;
		}else
		if (LBData.y == 2) {
			isEnemy = 1;
			wData = LBData.w;
		}else
		if (MBData.y == 2) {
			isEnemy = 1;
			wData = MBData.w;
		}else
		if (RBData.y == 2) {
			isEnemy = 1;
			wData = RBData.w;
		}

		if (isEnemy == 1) {
			if (wData == 1.0f) {
				output.color.xyzw = float32_t4(1.0f, 1.0f, 0.0f, 1.0f);
			}
			else if (wData == 2.0f) {

			}
			else {
				output.color.xyzw = float32_t4(0.5f, 0.0f, 0.5f, 1.0f);
			}
		}
		
	}
	else {
		output.color = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	return output.color;
}