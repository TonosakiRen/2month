#include "NormalLightEnemy.h"
#include "ShadowSpotLights.h"

NormalLightEnemy::~NormalLightEnemy() {
	if (shadowSpotLights_) {
		shadowSpotLights_ = nullptr;
	}
}

void NormalLightEnemy::SetLight(ShadowSpotLights* shadowlight, const uint32_t& num) {
	shadowSpotLights_ = shadowlight;
	number_ = num;
	auto& light = shadowSpotLights_->lights_.at(number_);
	light.worldTransform.SetParent(&worldTransform_);
	offset = Vector3(0.0f, -1.1f, -1.0f);
	light.distance = 30.0f;
}

void NormalLightEnemy::Update(const Vector3& playerPosition) {
	playerPosition_ = playerPosition;
	float distance = Distance(playerPosition, worldTransform_.translation_);
	
	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		isActive_ = false;
		return;
	}
	isActive_ = true;

	if (!isHit_) {
		lightDirection_ = Move(playerPosition);
	}
	else {
		CollisionProcess();
	}

	collider_.AdjustmentScale();
	UpdateTransform();
	if (shadowSpotLights_) {
		MoveLight();
	}
}

void NormalLightEnemy::MoveLight() {
	auto& light = shadowSpotLights_->lights_.at(number_);
	light.worldTransform.translation_ = offset;
	light.direction = lightDirection_;
}

void NormalLightEnemy::CollisionProcess() {
	// 死亡した時
	if (hp_ <= 0) {
		DownAnimation();
		return;
	}

	// 衝突時にノックバックとアニメーション
	KnockBack();
}
