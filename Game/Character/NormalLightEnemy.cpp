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
	//MoveLight();
	auto& light = shadowSpotLights_->lights_.at(number_);
	light.worldTransform.SetParent(&worldTransform_);
	light.worldTransform.SetIsRotateParent(false);
}

void NormalLightEnemy::Update(const Vector3& playerPosition) {
	playerPosition_ = playerPosition;
	float distance = Distance(playerPosition, worldTransform_.translation_);
	ImGui::Begin("Enemy");
	DrawImGui();
	ImGui::End();

	const float kMaxDistance = 50.0f;
	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		return;
	}

	if (!isHit_) {
		Move(playerPosition);
	}
	else {
		KnockBack();
	}

	collider_.AdjustmentScale();
	UpdateTransform();
	if (shadowSpotLights_) {
		MoveLight();
	}
}

void NormalLightEnemy::MoveLight() {
	auto& light = shadowSpotLights_->lights_.at(number_);
	light.worldTransform.translation_ = Vector3(0.0f, 0.0f, 1.5f);
	light.worldTransform.quaternion_ = worldTransform_.quaternion_;
	//light.direction = 
}
