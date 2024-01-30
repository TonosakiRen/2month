#include "BaseSpawner.h"
#include "ModelManager.h"

void BaseSpawner::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"woodbox", // 親
	};

	BaseInitialize(1, names);

	collider_.Initialize(&worldTransform_, "Spawner", models_.at(0).modelHandle_);

	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;


	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(models_.at(0).modelHandle_);
	// とりあえず一個だけ
	modelsTransform_.at(0).SetParent(&worldTransform_);
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();
}

void BaseSpawner::Update(const Vector3& playerPosition) {
	float distance = Distance(playerPosition, worldTransform_.translation_);

	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		isActive_ = false;
		return;
	}
	isActive_ = true;


	collider_.AdjustmentScale();
	UpdateTransform();
}

void BaseSpawner::OnCollision(Collider& collider, const PlayerDate& date) {

}

void BaseSpawner::Draw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	collider_.Draw();
	BaseDraw();
}

void BaseSpawner::EnemyDraw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	BaseEnemyDraw();
}

void BaseSpawner::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	UpdateTransform();
#endif // _DEBUG
}
