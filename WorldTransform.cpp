#include "WorldTransform.h"

void WorldTransform::Initialize() {
    constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
    Update();
}

void WorldTransform::Update() {

    // スケール、回転、平行移動行列の計算
    Matrix4x4 scaleMatrix = MakeScaleMatrix(scale_);

    Matrix4x4 rotateMatrix = MakeIdentity4x4();
    rotateMatrix = MakeRotateMatrix(quaternion_);
    Matrix4x4 translationMatrix = MakeTranslateMatrix(translation_);

    // ワールド行列の合成
    matWorld_ = MakeIdentity4x4();
    matWorld_ *= scaleMatrix;
    matWorld_ *= rotateMatrix;
    matWorld_ *= translationMatrix;


    // 親行列の指定がある場合は、掛け算する
    if (parent_) {
        //scaleを反映させない
        Matrix4x4 inverseMatrix = MakeIdentity4x4();

        if (!isScaleParent_) {
            inverseMatrix = Inverse(MakeScaleMatrix(MakeScale(parent_->matWorld_)));
            matWorld_ *= inverseMatrix;
        }

        if (!isRotateParent_) {
            inverseMatrix = Inverse(NormalizeMakeRotateMatrix(parent_->matWorld_));
            matWorld_ *= inverseMatrix;
        }

        matWorld_ *= parent_->matWorld_;
    }

    if (constBuffer_.GetCPUData()) {
        constBuffer_.Copy(matWorld_);
    }
}
