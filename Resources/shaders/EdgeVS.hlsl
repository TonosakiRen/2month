
Texture2D<float4> colorTex : register(t0);

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

VSOutput main(float32_t4 pos : POSITION, float32_t2 uv : TEXCOORD0) {
	VSOutput output;

	float32_t2 texSize;
	//テクスチャーのサイズ
	colorTex.GetDimensions(texSize.x, texSize.y);

	output.svpos = pos;
	float32_t2 tex = uv;

	float32_t2 pixelSize = 1.0f / texSize;

	//法線
	{
		//真ん中のピクセル
		output.texCenter = tex;
		//右上のピクセル
		output.texLT.xy = tex + float32_t2(pixelSize.x , -pixelSize.y);
		//上のピクセル
		output.texMT.xy = tex + float32_t2(0.0f, -pixelSize.y);
		//左上のピクセル
		output.texRT.xy = tex + float32_t2(-pixelSize.x , -pixelSize.y);
		//右のピクセル
		output.texLM.xy = tex + float32_t2(pixelSize.x,0.0f);
		//左のピクセル
		output.texRM.xy = tex + float32_t2(-pixelSize.x,0.0f);
		//上のピクセル
		output.texLB.xy = tex + float32_t2(pixelSize.x , pixelSize.y);
		//上のピクセル
		output.texMB.xy = tex + float32_t2(0.0f, pixelSize.y);
		//上のピクセル
		output.texRB.xy = tex + float32_t2(-pixelSize.x , pixelSize.y);

	}



	return output;
}