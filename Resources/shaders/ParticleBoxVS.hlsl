
struct WorldTransform {
	float32_t4x4 world;
};
StructuredBuffer<WorldTransform> gWorldTransforms  : register(t0);

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct VSInput {
	float32_t4 pos : POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	uint32_t instanceId : SV_InstanceID;
};

struct VSOutput {
	float32_t4 svpos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	float32_t3 worldPosition : POSITION;
};

VSOutput main(VSInput input) {
	VSOutput output;
	output.svpos = mul(input.pos, mul(gWorldTransforms[input.instanceId].world, gViewProjection.viewProjection));
	output.normal = mul(normal, (float32_t3x3)gWorldTransform[input.instanceId].world);
	output.uv = input.uv;
	output.worldPosition = mul(input.pos, gWorldTransforms[input.instanceId].world).xyz;
	return output;
}