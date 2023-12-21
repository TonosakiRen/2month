#include "Material.h"
#include "externals/DirectXTex/d3dx12.h"

void Material::Initialize() {
    constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
    uvTransform_ = MakeIdentity4x4();
    Update();
}


void Material::Update() {
    Matrix4x4 matScale;
    Matrix4x4 matRot;
    Matrix4x4 matTrans;
    // スケール、回転、平行移動行列の計算
    matScale = MakeScaleMatrix(scale_);
    matRot = MakeIdentity4x4();
    matRot *= MakeRotateZMatrix(rotation_.z);
    matRot *= MakeRotateXMatrix(rotation_.x);
    matRot *= MakeRotateYMatrix(rotation_.y);
    matTrans = MakeTranslateMatrix(translation_);

    // ワールド行列の合成
    uvTransform_ = MakeIdentity4x4();
    uvTransform_ *= matScale;
    uvTransform_ *= matRot;
    uvTransform_ *= matTrans;

    // 定数バッファに書き込み
    ConstBufferData bufferData;
    bufferData.color = color_;
    bufferData.uvTransform = uvTransform_;
    bufferData.enableLighting = static_cast<uint32_t>(enableLighting_);
    constBuffer_.Copy(bufferData);
}