struct WorldTransform {
	float32_t4x4 world; 
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(t0);

struct ViewProjection {
	float32_t4x4 view;      
	float32_t4x4 projection; 
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct Material {
	float32_t4 materialcolor; 
	float32_t4x4 uvTransfrom;
};
ConstantBuffer<Material> gMaterial  : register(b1);

struct VSOutput {
	float32_t4 svpos : SV_POSITION; 
	float32_t2 uv : TEXCOORD;     
};
