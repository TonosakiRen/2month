#include "NormalSpawner.h"
#include "NormalEnemy.h"
#include <memory>
#include "ModelManager.h"
#include "Compute.h"
#include "Player.h"


void NormalSpawner::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate, const uint32_t& interval, const uint32_t& MaxPop, const int& hp) {
	std::vector<std::string> names = {
		"woodbox", // 親
	};

	BaseInitialize(static_cast<uint32_t>(names.size()), names);

	collider_.Initialize(&worldTransform_, "Spawner", models_.at(0).modelHandle_);

	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	popInterval_ = interval;
	kMaxPopEnemy_ = MaxPop;
	hp_ = hp;
	color_ = Vector4(0.95f, 0.76f, 0.20f, 1.0f);

	// とりあえず一個だけ
	modelsTransform_.at(0).parent_ = &worldTransform_;
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	modelsTransform_.at(0).Update();

	InsertData();
}

bool NormalSpawner::UpdateSpawn(const Vector3& playerPosition) {
	float distance = Distance(playerPosition, worldTransform_.translation_);
	// Playerとの距離が一定数以下なら早期リターン
	if (distance > kMaxDistance) {
		isActive_ = false;
		return false;
	}
	isActive_ = true;

	if (isHit_) {
		// 当たった時の処理
		isHit_ = false;
		CollisionProcess();
	}

	collider_.AdjustmentScale();
	UpdateTransform();
	
	return EnemySpawn();
}

void NormalSpawner::OnCollision(Collider& collider, const PlayerDate& date) {
	if (!isActive_) { return; }

	// 攻撃Idがリセットされているのであれば都度リセット
	if (date.id == 0u) {
		id_ = date.id;
	}
	if (!isActive_) { return; }
	else if (hp_ <= 0) { return; }

	bool isColl = false;

	uint32_t* shadowDate = static_cast<uint32_t*>(Compute::GetData());
	if (shadowDate[kNumber_] == 1) {
		Player::hitShadowEnemyIndex_ = kNumber_;
		Player::hitShadowEnemyPos_ = MakeTranslation(worldTransform_.matWorld_);
		Player::hitReaction_ = Player::damage;
		isColl = true;
	}

	Vector3 pushBackVector;
	if (collider_.Collision(collider, pushBackVector)) {
		isColl = true;
		Player::hitCollider_ = &collider_;
		Player::hitReaction_ = Player::knockBack;
	}

	if (isColl) {
		if (!isHit_ && date.isAttack_ && (id_ != date.id)) {
			// 当たった瞬間
			isHit_ = true;
			id_ = date.id;
			hp_ -= date.damage_;
		}
		UpdateTransform();
	}
}

void NormalSpawner::Draw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	collider_.Draw();
	models_.at(0).Draw(modelsTransform_.at(0), color_);
}

void NormalSpawner::EnemyDraw() {
#ifndef _DEBUG
	if (!isActive_) { return; }
#endif // RELEASE
	BaseEnemyDraw();
}

void NormalSpawner::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	int intHandle = static_cast<int>(popInterval_); ImGui::DragInt("popInterval", &intHandle, 1); popInterval_ = static_cast<uint32_t>(intHandle);
	intHandle = static_cast<int>(kMaxPopEnemy_); ImGui::DragInt("MaxPopEnemy", &intHandle, 1); kMaxPopEnemy_ = static_cast<uint32_t>(intHandle);
	ImGui::DragInt("hp", &hp_, 1);
	UpdateTransform();
	InsertData();
#endif // _DEBUG
}

bool NormalSpawner::EnemySpawn() {
	if (++poptimer_ >= popInterval_) {
		if (popCount_ >= kMaxPopEnemy_) { 
			color_.y = 0.0f; color_.z = 0.0f;
			return false;
		}
		poptimer_ = 0u;
		respawnPoint_.scale = Vector3(1.0f, 1.0f, 1.0f);
		respawnPoint_.rotate = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		respawnPoint_.translate = worldTransform_.GetWorldTranslate();
		popCount_++;
		return true;
	}
	return false;
}

void NormalSpawner::InsertData() {
	respawnPoint_.scale = Vector3(1.0f, 1.0f, 1.0f);
	respawnPoint_.rotate = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	respawnPoint_.translate = worldTransform_.GetWorldTranslate();
	respawnPoint_.interval = static_cast<int>(popInterval_);
	respawnPoint_.MaxPopEnemy = static_cast<int>(kMaxPopEnemy_);
	respawnPoint_.hp = hp_;
}

void NormalSpawner::CollisionProcess() {
	if (hp_ <= 0u) {
		isAlive_ = false;
	}
	isHit_ = false;
}

