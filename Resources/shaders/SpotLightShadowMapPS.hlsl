struct VSOutput {
	float32_t4 pos : SV_POSITION; // システム用頂点座標
};

struct PixelShaderOutput {
	float32_t2 color : SV_TARGET0;
};

struct CollisionData {
	float32_t2 collisionData;
};
ConstantBuffer<CollisionData> gCollisionData  : register(b2);


PixelShaderOutput main(VSOutput input)
{

	PixelShaderOutput output;

	output.color = gCollisionData.collisionData;

	return output;
}