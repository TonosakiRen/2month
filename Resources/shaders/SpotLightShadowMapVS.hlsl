struct WorldTransform {
	float32_t4x4 world;
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);

struct ShadowSpotLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t3 direction;
	float32_t distance;
	float32_t decay;
	float32_t cosAngle;
	float32_t isActive;
	uint32_t shadowDescriptorIndex;
	uint32_t collisionDescriptorIndex;
	uint32_t padding;
	uint32_t padding1;
	uint32_t padding2;
	float32_t4x4 viewProjection;
};
ConstantBuffer<ShadowSpotLight> gShadowSpotLight  : register(b1);

struct VSOutput {
	float32_t4 svpos : SV_POSITION;
};

VSOutput main(float32_t4 pos : POSITION)
{
	VSOutput output;
	output.svpos = mul(pos, mul(gWorldTransform.world, gShadowSpotLight.viewProjection));

	return output;
}