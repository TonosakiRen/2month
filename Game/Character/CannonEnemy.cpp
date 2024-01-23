#include "CannonEnemy.h"

void CannonEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"enemy01", // 親
		"toge", // 弾
	};
	BaseInitialize(static_cast<uint32_t>(names.size()), names);
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	if (!modelsTransform_.empty()) { modelsTransform_.clear(); }
	const uint32_t kMaxBullet = 5;
	modelsTransform_.resize(kMaxBullet + 1u);

	// 親 大砲
	modelsTransform_.at(0).SetParent(&worldTransform_);
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();

	// 大砲の衝突判定
	auto& coll = colliders_.emplace_back(Collider());
	coll.Initialize(&worldTransform_, "Cannon", models_.at(0).modelHandle_);

	// 弾の衝突判定
	for (uint32_t index = 0u; index < kMaxBullet; index++) {
		auto& coll = colliders_.emplace_back(Collider());
		coll.Initialize(&worldTransform_, "Cannon", models_.at(1).modelHandle_);
	}
	
	UpdateTransform();
}

void CannonEnemy::Update(const Vector3& playerPosition) {
	float distance = Distance(playerPosition, worldTransform_.translation_);
	const float kMaxDistance = 50.0f;
	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		return;
	}
	for (auto& coll : colliders_) {
		coll.AdjustmentScale();
	}
	UpdateTransform();
}

void CannonEnemy::OnCollision(Collider& collider, const PlayerDate& date) {

}

void CannonEnemy::Draw() {
	for (auto& coll : colliders_) {
		coll.Draw();
	}
	BaseDraw();
}

void CannonEnemy::EnemyDraw() {
	BaseEnemyDraw();
}

void CannonEnemy::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 rotHandle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(rotHandle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	int handle = static_cast<int>(interval_); ImGui::DragInt("interval", &handle, 1); interval_ = static_cast<uint32_t>(handle);
	
	UpdateTransform();
#endif // _DEBUG
}
