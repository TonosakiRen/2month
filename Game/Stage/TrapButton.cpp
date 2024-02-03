#include "TrapButton.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Game/Character/BaseCharacter.h"
#undef min

decltype(TrapButton::masterNumber_) TrapButton::masterNumber_ = 0u;

TrapButton::TrapButton() : kNumber_(masterNumber_) {
	masterNumber_++;
}

TrapButton::~TrapButton() {

}

void TrapButton::Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate) {
	std::string name = "box1x1";
	GameObject::Initialize(name);
	collider_.Initialize(&worldTransform_, name, modelHandle_);
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	UpdateMatrix();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;
}

void TrapButton::Update(const Vector3& playerWorldPosition) {
	if (!ActiveChack(playerWorldPosition)) {
		isActive_ = false;
		return;
	}
	isActive_ = true;

	collider_.AdjustmentScale();
	UpdateMatrix();
}

void TrapButton::Draw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	collider_.Draw();
	GameObject::Draw();
}

void TrapButton::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);

	UpdateMatrix();
#endif // _DEBUG
}

bool TrapButton::ActiveChack(const Vector3& playerWorldPosition) const {
	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	Vector3 minSize = worldTransform_.GetWorldTranslate() - modelSize;
	Vector3 maxSize = worldTransform_.GetWorldTranslate() + modelSize;

	if ((minSize.x <= playerWorldPosition.x) && (playerWorldPosition.x <= maxSize.x)) {
		// モデルのx軸の間にいるので何もせず終わり
	}
	else {
		float distance = std::min(Distance(playerWorldPosition, minSize), Distance(playerWorldPosition, maxSize));
		// Playerとの距離が一定数以下なら早期リターン
		if (distance > kMaxDistance) {
			return false;
		}
	}
	return true;
}
