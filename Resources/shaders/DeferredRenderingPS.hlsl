
struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct DirectionLight {
	float32_t4 color;     //ライトの色
	float32_t3 direction; //ライト向き
	float32_t intensity;   //輝度
};
ConstantBuffer<DirectionLight> gDirectionLight  : register(b1);

float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
	// xは0~1から-1~1, yは0~1から1~-1に上下反転
	float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	float4 position = float4(xy, depth, 1.0f);
	position = mul(position, viewProjectionInverseMatrix);
	position.xyz /= position.w;
	return position.xyz;
}

Texture2D<float4> colorTex : register(t0);
Texture2D<float4> normalTex : register(t1);
Texture2D<float4> depthTex : register(t2);

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	PixelShaderOutput output;

	float32_t3 color = colorTex.Sample(smp, input.uv).xyz;
	float32_t3 normal = normalTex.Sample(smp, input.uv).xyz;
	normal = normal * 2.0f - 1.0f;
	float32_t depth = depthTex.Sample(smp, input.uv).x;

	float32_t3 worldPos = GetWorldPosition(input.uv, depth, gViewProjection.inverseViewProjection);

	//陰
	float32_t NdotL = dot(normal, -normalize(gDirectionLight.direction));
	float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 diffuse = gDirectionLight.color.xyz * cos * gDirectionLight.intensity;

	//反射
	float32_t3 viewDirection = normalize(gViewProjection.viewPosition - worldPos.xyz);

	float32_t3 reflectVec = reflect(gDirectionLight.direction, normal);
	float32_t3 speculerColor = float32_t3(1.0f, 1.0f, 1.0f);
	float32_t specluerPower = 10.0f;
	float32_t3 specluer = speculerColor * pow(saturate(dot(reflectVec, viewDirection)), specluerPower);

	//アンビエント
	float32_t3 ambient = float32_t3(0.3f, 0.3f, 0.3f);


	color.xyz *= (diffuse + specluer + ambient);

	output.color.xyz = color.xyz;

	return output.color;
}