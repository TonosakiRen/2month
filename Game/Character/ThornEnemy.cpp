#include "ThornEnemy.h"
#include "ModelManager.h"
#include "Compute.h"
#include "Player.h"

void ThornEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"toge", // 親
	};

	BaseInitialize(1, names);
	collider_.Initialize(&worldTransform_, "toge", models_.at(0).modelHandle_);

	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(models_.at(0).modelHandle_);

	// とりあえず一個だけ
	modelsTransform_.at(0).parent_ = &worldTransform_;
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	//modelsTransform_.at(0).translation_.y += modelSize.y / 2.0f;
	modelsTransform_.at(0).Update();

	amplitude_ = Vector3(1.0f, 0.0f, 0.0f);

}

void ThornEnemy::Update(const Vector3& playerPosition) {
	float distance = Distance(playerPosition, worldTransform_.GetWorldTranslate());
	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		isActive_ = false;
		return;
	}
	isActive_ = true;

	Move();

	collider_.AdjustmentScale();
	UpdateTransform();
}

void ThornEnemy::OnCollision(Collider& collider, const PlayerDate& date) {
	
	bool isColl = false;

	uint32_t* shadowDate = static_cast<uint32_t*>(Compute::GetData());
	if (shadowDate[kNumber_] == 1) {
		Player::hitShadowEnemyIndex_ = kNumber_;
		Player::hitShadowEnemyPos_ = MakeTranslation(worldTransform_.matWorld_);
		Player::hitReaction_ = Player::damage;
		isColl = true;
	}

	Vector3 pushBackVector;
	if (collider_.Collision(collider, pushBackVector) && !shadowOnly_) {
		isColl = true;
		Player::hitReaction_ = Player::damage;
		Player::hitCollider_ = &collider_;
		Player::hitShadowEnemyPos_ = MakeTranslation(worldTransform_.matWorld_);
	}

	if (isColl) {
		
		UpdateTransform();
	}

}

void ThornEnemy::Draw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // _RELEASE
	collider_.Draw();
	BaseDraw();
}

void ThornEnemy::EnemyDraw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // _RELEASE
	BaseEnemyDraw();
}

void ThornEnemy::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("amplitude", &amplitude_.x, 0.1f);
	ImGui::DragInt("Count", &kMaxTime_, 1, 1, 1000);
	if (ImGui::Button("isDraw")) {
		shadowOnly_ = !shadowOnly_;
	}
	UpdateTransform();
#endif // _DEBUG
}

void ThornEnemy::SetState(const Vector3& amplitube, const uint32_t time) {
	amplitude_ = amplitube;
	kMaxTime_ = time;
	timer_ = time / 2u;
}

void ThornEnemy::Move() {
	Vector3 moveVec;

	if (timer_++ > static_cast<uint32_t>(kMaxTime_)) {
		isMovingToWhich = !isMovingToWhich;
		timer_ = 0u;
	}

	const Vector3 velocity = amplitude_ / static_cast<float>(kMaxTime_);
	isMovingToWhich ? moveVec += velocity : moveVec -= velocity;
	worldTransform_.translation_ += moveVec;
}
