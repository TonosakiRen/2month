struct WorldTransform  {
	float32_t4x4 world; // ワールド変換行列
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);

struct ViewProjection  {
	float32_t4x4 view;       // ビュー変換行列
	float32_t4x4 projection; // プロジェクション変換行列
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

struct DirectionLight  {
	float32_t4 color;     //ライトの色
	float32_t3 direction; //ライト向き
	float32_t intensity;   //輝度
};
ConstantBuffer<DirectionLight> gDirectionLight  : register(b2);

struct Material  {
	float32_t4 materialcolor; //Materialの色
	float32_t4x4 uvTransfrom;//uvtransform
};
ConstantBuffer<Material> gMaterial  : register(b3);

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput {
	float32_t4 svpos : SV_POSITION; // システム用頂点座標
	float32_t3 normal : NORMAL;     // 法線ベクトル
	float32_t2 uv : TEXCOORD;       // uv値
};
