#include "MoveFloor.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Game/Character/BaseCharacter.h"
#undef min

void MoveFloor::Initialize(const Vector3& startPos, const Vector3& endPos, const Quaternion& quaternion, const Vector3& scale, const float& speed) {
	std::string name = "stagefloor";
	GameObject::Initialize(name);
	collider_.Initialize(&worldTransform_, "movefloor", modelHandle_);
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = startPos;
	UpdateMatrix();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	moveParam_.startPos_ = startPos;
	moveParam_.endPos_ = endPos;
	moveParam_.speed_ = speed;
	moveParam_.T = 0.0f;
}

void MoveFloor::Update(const Vector3& playerWorldPosition) {
	if (!ActiveChack(playerWorldPosition)) {
		isActive_ = false;
		return;
	}
	isActive_ = true;

	Move();

	collider_.AdjustmentScale();
	UpdateMatrix();
}

void MoveFloor::Draw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	collider_.Draw();
	GameObject::Draw();
}

void MoveFloor::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("start", &moveParam_.startPos_.x, 0.1f);
	ImGui::DragFloat3("end", &moveParam_.endPos_.x, 0.1f);
	ImGui::DragFloat("speed", &moveParam_.speed_, 0.1f);
	
	worldTransform_.translation_ = moveParam_.startPos_;

	UpdateMatrix();
#endif // _DEBUG
}

void MoveFloor::Move() {
	static bool flag = true;

	if (moveParam_.T >= 1.0f) {
		flag = false;
		moveParam_.T = 1.0f;
	}
	else if (moveParam_.T <= 0.0f) {
		flag = true;
		moveParam_.T = 0.0f;
	}

	if (flag) {
		moveParam_.T += 1.0f / moveParam_.speed_;
	}
	else {
		moveParam_.T -= 1.0f / moveParam_.speed_;
	}


	worldTransform_.translation_ = Lerp(moveParam_.startPos_, moveParam_.endPos_, moveParam_.T);
	
}

bool MoveFloor::ActiveChack(const Vector3& playerWorldPosition) const {
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
