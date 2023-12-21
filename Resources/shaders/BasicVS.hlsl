
struct WorldTransform {
	float32_t4x4 world; 
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

struct VSOutput {
	float32_t4 svpos : SV_POSITION; 
	float32_t3 normal : NORMAL;     
	float32_t2 uv : TEXCOORD;   
	float32_t3 worldPosition : POSITION;
};

VSOutput main(float32_t4 pos : POSITION, float32_t3 normal : NORMAL, float32_t2 uv : TEXCOORD) {
	VSOutput output; // ピクセルシェーダーに渡す値
	output.svpos = mul(pos, mul(gWorldTransform.world, gViewProjection.viewProjection));
	output.normal = mul(normal, (float32_t3x3)gWorldTransform.world);
	output.uv = uv;
	output.worldPosition = mul(pos, gWorldTransform.world).xyz;
	return output;
}