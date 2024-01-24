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
};

VSOutput main(float32_t4 pos : POSITION)
{
	VSOutput output;
	output.svpos = mul(pos, mul(gWorldTransform.world, gViewProjection.viewProjection));

	return output;
}