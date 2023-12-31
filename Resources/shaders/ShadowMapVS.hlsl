struct WorldTransform {
	float32_t4x4 world;
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);

struct DirectionLight {
	float32_t4 color;
	float32_t3 direction;
	float32_t intensity;
	float32_t4x4 viewProjection;
};
ConstantBuffer<DirectionLight> gDirectionLight  : register(b1);

struct VSOutput {
	float32_t4 svpos : SV_POSITION;
};

VSOutput main(float32_t4 pos : POSITION)
{
	VSOutput output;
	output.svpos = mul(pos, mul(gWorldTransform.world, gDirectionLight.viewProjection));

	return output;
}